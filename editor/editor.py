import Tkinter
from Tkinter import *
import ScrolledText as tkst
import tkFileDialog as tkfd
import tkMessageBox as tkmb

root = Tk(className=" Just another Text Editor")
textPad = tkst.ScrolledText(root, width=100, height=80)

# create a menu & define functions for each menu item

def open_command():
        file = tkfd.askopenfile(parent=root,mode='rb',title='Select a file')
        if file != None:
            contents = file.read()
            textPad.insert('1.0',contents)
            file.close()

def file_save():
    f = tkfd.asksaveasfile(mode='w', defaultextension=".txt")
    if f is None: # asksaveasfile return `None` if dialog closed with "cancel".
        return
    text2save = str(textPad.get(1.0, END)) # starts from `1.0`, not `0.0`
    f.write(text2save)
    f.close() # `()` was missing.

def exit_command():
    if tkmb.askokcancel("Quit", "Do you really want to quit?"):
        root.destroy()

def about_command():
    label = tkmb.showinfo("About", "Just Another TextPad \n Copyright \n No rights left to reserve")


def dummy():
    print "I am a Dummy Command, I will be removed in the next step"
menu = Menu(root)
root.config(menu=menu)
filemenu = Menu(menu)
menu.add_cascade(label="File", menu=filemenu)
filemenu.add_command(label="New", command=dummy)
filemenu.add_command(label="Open...", command=open_command)
filemenu.add_command(label="Save", command=file_save)
filemenu.add_separator()
filemenu.add_command(label="Exit", command=exit_command)
helpmenu = Menu(menu)
menu.add_cascade(label="Help", menu=helpmenu)
helpmenu.add_command(label="About...", command=about_command)

#
textPad.pack()
root.mainloop()
