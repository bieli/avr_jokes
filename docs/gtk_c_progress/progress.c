/*
gcc -Wall -g `pkg-config --cflags --libs gtk+-2.0 gmodule-export-2.0 gthread-2.0 libcurl` liststore2.c -o liststore2
 */
#include <stdio.h>
#include <gtk/gtk.h>
#include <glib.h>
#include <unistd.h>
#include <pthread.h>

#include <curl/curl.h>
#include <curl/types.h> /* new for v7 */
#include <curl/easy.h> /* new for v7 */

gchar *URL = "http://soundclash-records.co.uk/mp3s/upfull_rockers_never_gonna_let_you_down.mp3";
static GHashTable* TreeRowReferences;
static GPrivate* current_data_key = NULL;

size_t my_write_func(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    return fwrite(ptr, size, nmemb, stream);
}

size_t my_read_func(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
    return fread(ptr, size, nmemb, stream);
}

typedef struct _Data Data;
struct _Data
{
    GtkWidget *down; /* Down button */
    GtkWidget *tree; /* Tree view */
    gdouble progress;
};

enum
{
    STRING_COLUMN,
    INT_COLUMN,
    N_COLUMNS
};

gboolean set_download_progress(gpointer data)
{
    Data *treeview = (Data *)data;

    GtkListStore* store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(treeview->tree)));
    GtkTreeIter iter;

    GtkTreeRowReference* reference = g_hash_table_lookup(TreeRowReferences,data);

    GtkTreePath* path = gtk_tree_row_reference_get_path(reference);

    gtk_tree_model_get_iter(GTK_TREE_MODEL(store), 
                                &iter, path);

    gtk_list_store_set(store, &iter,
                       INT_COLUMN,treeview->progress, -1);

    gtk_tree_path_free (path);
    return FALSE;
}

int my_progress_func(Data *data,
             double t, /* dltotal */
             double d, /* dlnow */
             double ultotal,
             double ulnow)
{
    if(t == 0)
        return 0;
    data->progress = d*100.0/t;

    gdk_threads_enter();
    g_idle_add(set_download_progress, data);
    gdk_threads_leave();
    return 0;
}

void *create_thread(void *data)
{

    Data *current_treeview = g_private_get (current_data_key);

    if (!current_treeview)
    {
      current_treeview = g_new (Data, 1);
      current_treeview = (Data *)data;
      g_private_set (current_data_key, current_treeview);
      g_print("p %g\n",current_treeview->progress);
    }
    else{
        current_treeview = (Data *)data;
        g_print("c %g\n",current_treeview->progress);
    }

    g_print("url\n");
    CURL *curl;
    CURLcode res;
    FILE *outfile;
    gchar *url = URL;
    gdk_threads_enter();
    curl = curl_easy_init();
    if(curl)
    {
        outfile = fopen("test.curl", "w");
        if(outfile)
                g_print("curl\n");

        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, outfile);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, my_write_func);
        curl_easy_setopt(curl, CURLOPT_READFUNCTION, my_read_func);

        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, FALSE);
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, my_progress_func);
        curl_easy_setopt(curl, CURLOPT_PROGRESSDATA, current_treeview);
        gdk_threads_leave();
        res = curl_easy_perform(curl);

        fclose(outfile);
        /* always cleanup */
        curl_easy_cleanup(curl);
    }
    return NULL;
}


G_MODULE_EXPORT void
cb_add( GtkWidget *button,
         Data      *data )
{    
    Data *current_download = (Data *)data;
    GtkTreeIter iter;
    GtkListStore* store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(current_download->tree)));
    gtk_list_store_append( store, &iter );

    GtkTreeRowReference* reference = NULL;
    GtkTreePath* path = gtk_tree_model_get_path(GTK_TREE_MODEL(store), &iter);
    reference = gtk_tree_row_reference_new(GTK_TREE_MODEL(store), path);
    g_hash_table_insert(TreeRowReferences, current_download, reference);
    gtk_tree_path_free(path);

    if (!g_thread_create(&create_thread, current_download, FALSE, NULL) != 0)
        g_warning("can't create the thread");
} 

int main(int argc, char **argv)
{
    GtkBuilder *builder;
    GtkWidget  *window;
    Data       *data;


    curl_global_init(CURL_GLOBAL_ALL);
    if( ! g_thread_supported() )
        g_thread_init( NULL );

    gdk_threads_init();
    gtk_init(&argc, &argv);

    data = g_slice_new( Data );

    /* Create builder */
    builder = gtk_builder_new();
    gtk_builder_add_from_file( builder, "progress.glade", NULL );

    window    = GTK_WIDGET( gtk_builder_get_object( builder, "window1" ) );
    data->down   = GTK_WIDGET( gtk_builder_get_object( builder, "down" ) );
    data->tree = GTK_WIDGET( gtk_builder_get_object( builder, "treeview" ) );
    TreeRowReferences = g_hash_table_new(NULL, NULL);

    gtk_builder_connect_signals( builder, data );
    g_object_unref( G_OBJECT( builder ) );

    gdk_threads_enter();
    gtk_widget_show( window );
    gdk_threads_leave();

    gtk_main();
    g_slice_free( Data, data );

    return 0;
}

