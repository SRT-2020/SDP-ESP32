from tkinter import *
import os
  
# Create object
root = Tk()
root.title("Trash-E: Set Run Time")

  
# Adjust size
root.geometry( "400x400" )

frame = Frame(master=root, width=150, height=150)
frame.pack()
  
# Change the label text
def show():

    day = clicked_day.get()
    time = clicked_time.get()
    label.config( text = "Your Garbage Will be Picked Up on {} at {}!".format(day, time))
    if(day == "Monday"):
        day_digit = '1'
    if(day == "Tuesday"):
        day_digit = '2'
    if(day == "Wednesday"):
        day_digit = '3'
    if(day == "Thursday"):
        day_digit = '4'
    if(day == "Friday"):
        day_digit = '5'
    if(day == "Saturday"):
        day_digit = '6'
    if(day == "Sunday"):
        day_digit = '0'


    if(time == "12:00pm"):
        time_digit = '12'
    elif(time[5:] == 'pm'):
        military_time = int(time[:2])
        military_time += 12
        time_digit = str(military_time)
    elif(time == "12:00am"):
        time_digit = '00'
    else:
        time_digit = time[:2]

    runtime = day_digit + time_digit

    f = open("time.html", "a")
    send_string = "<p>" + runtime + "</p>"
    f.write(send_string)
    f.close() 

    os.system("curl -X POST 192.168.1.2:80/delete/time.html")
    os.system("curl -X POST --data-binary @time.html 192.168.1.2:80/upload/time.html")

    os.remove("time.html")

    label2.config( text = "Time Set Successful")





  
# Dropdown menu options
options_day = [
    "Sunday",
    "Monday",
    "Tuesday",
    "Wednesday",
    "Thursday",
    "Friday",
    "Saturday"   
]
  
# datatype of menu text
clicked_day = StringVar()
  
# initial menu text
clicked_day.set( "Trash Pickup Day" )
  
# Create Dropdown menu
drop_day = OptionMenu( root , clicked_day , *options_day)
drop_day.pack()

options_time = [
    "12:00am",
    "01:00am",
    "02:00am",
    "03:00am",
    "04:00am",
    "05:00am",
    "06:00am",
    "07:00am",
    "08:00am",
    "09:00am",
    "10:00am",
    "11:00am",
    "12:00pm",
    "01:00pm",
    "02:00pm",
    "03:00pm",
    "04:00pm",
    "05:00pm",
    "06:00pm",
    "07:00pm",
    "08:00pm",
    "09:00pm",
    "10:00pm",
    "11:00pm"
]
  
# datatype of menu text
clicked_time = StringVar()
  
# initial menu text
clicked_time.set( "Trash Pickup Time" )
  
# Create Dropdown menu
drop_time = OptionMenu( root , clicked_time , *options_time )
drop_time.pack()
  
# Create button, it will change label text
button_time = Button( root , text = "Set" , command = show ).pack()
  
# Create Label
label = Label( root , text = " " )
label.pack()

label2 = Label( root, text = " ")
label2.pack()
  
# Execute tkinter
root.mainloop()