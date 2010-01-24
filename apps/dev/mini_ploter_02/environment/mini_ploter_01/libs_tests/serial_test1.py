# Python PC app to talk to an ATmega32 uP via RS-232
# Author: davef
# Date:   26 Dec 2006

# Notes:
# timeout = None, waits FOREVER in it's own thread

# PC_control.bind("<Button-1>", writer), doesn't need the .bind. The left
#     mouse button is automagically connected to command by default

# def __init__(self, master, queue, endApplication):
# queue is an object from ThreadedClient. It needs be passed to GuiPart
#     and declared in this "user" class.

# PC_display needs to be self.PC_display because:
# When accessing data attributes within a class, you need to qualify the
# attribute name: self.attribute. When calling other methods within a class,
# you need to qualify the method name: self.method.

# Todo:
# check that serial_comms.close() in def serial_commsThread(self); works

# def writer(), can't pass in parameters.  See "the extra arguments trick" OR
# lambda, currying and event_lambda


import Tkinter
import threading
import Queue
import serial
       

class GuiPart:
    def __init__(self, master, queue, endApplication):
        self.queue = queue

      # Set up the GUI

        self.myContainer1 = Tkinter.Frame(master)
        self.myContainer1.pack()
       
        self.PC_display = Tkinter.Label(self.myContainer1,\
                          text = 'AVR switch setting is unknown',\
                          background = 'white')
        self.PC_display.pack()

        self.PC_control = Tkinter.Button(self.myContainer1, text = 'Toggle AVR switch',\
                          background = 'white',command = self.send_msg1)
        self.PC_control.pack()

        self.PC_control2 = Tkinter.Button(self.myContainer1, text = 'Test option 2',\
                          background = 'gray',command = self.send_msg2)
        self.PC_control2.pack()


        self.quitButton = Tkinter.Button(self.myContainer1, text='QUIT?', fg = 'red',\
                          command = endApplication)
        self.quitButton.pack()


    def send_msg1(self):
        #serial_comms.write('You toggled the PC switch')     
	serial_comms.write('1')     

    def send_msg2(self):
	serial_comms.write('2')

    def processIncoming(self):
        """
        Handle all the messages currently in the queue (if any).
        """
        while self.queue.qsize():
            try:
                AVR_push_button = self.queue.get()
		"""
                if AVR_push_button == 'f':
                     self.PC_display['text'] = 'AVR switch is OFF'
                     self.PC_display['background'] = 'red'

                elif AVR_push_button == 'd':
                     self.PC_display['text'] = 'AVR switch is ON'
                     self.PC_display['background'] = 'light green'
		"""
		
		self.PC_display['text'] = AVR_push_button
 		self.PC_display['background'] = 'light green'

                print AVR_push_button # to the DOS command window

            except Queue.Empty:
		print "queue EMpty"
                pass


class ThreadedClient:
    """
    Launch the main part of the GUI and the worker thread. periodicCall and
    endApplication could reside in the GUI part, but putting them here
    means that you have all the thread controls in a single place.
    """
    def __init__(self, master):
        """
        Start the GUI and the asynchronous threads. We are in the main
        (original) thread of the application, which will later be used by
        the GUI. We spawn a new thread for the worker.
        """
        self.master = master

      # Create the queue
        self.queue = Queue.Queue()

      # Set up the GUI part
        self.gui = GuiPart(master, self.queue, self.endApplication)
               
      # Set up the thread to do asynchronous I/O
        self.running = 1
        self.thread1 = threading.Thread(target = self.serial_commsThread)
        self.thread1.setDaemon(1) # correction to original
        self.thread1.start()

      # Start the periodic call in the GUI to check if the queue contains anything
        self.periodicCall()


    def periodicCall(self):
        """
        Check every 100 ms if there is something new in the queue.
        """
        self.gui.processIncoming()
        if not self.running:
          # This is the brutal stop of the system. You may want to do
          # some cleanup before actually shutting it down.
           # import sys
           # sys.exit(1)

           # root.quit() # suggested modification
            root.destroy() # works better using IDLE
           
        self.master.after(100, self.periodicCall)


    def serial_commsThread(self):
        """
        process the asynchronous I/O (serial comms).
        One important thing to remember is that the thread has to yield
        control.
        """
        while self.running:
            receive_msg = serial_comms.read()
	    #print " reciveMSG : " % receive_msg
            self.queue.put(receive_msg)
        else:
            serial_comms.close()
           

    def endApplication(self):
        self.running = 0
           

serial_comms = serial.Serial(port = 0, baudrate = 9600, bytesize = serial.EIGHTBITS,\
               parity = serial.PARITY_NONE, stopbits = serial.STOPBITS_ONE,\
               timeout = None, rtscts = 0, xonxoff = 0)



if __name__ == "__main__":
    root = Tkinter.Tk()
    root.title('AVR control')
    client = ThreadedClient(root)
    root.mainloop()
