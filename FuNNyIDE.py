VERSION = '1.0'
V_DATE = '16/8 2023'

import os
import subprocess
import sys
from datetime import datetime
from tkinter import font as tkfont

import PySimpleGUI as sg

import lex


class Multiline(sg.Multiline):

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.ratio = 0
        self.lines = 1

    def initial(self, window, width=3, bg='#202020', fg='white', font=None):
        self.window = window
        self.line = sg.tk.Text(self.widget.master, width=3, height=self.Size[1], bg=bg, fg=fg, font=font)
        #self.line = sg.tk.Text(self, width=3, height=self.Size[1], bg=bg, fg=fg, font=font)
        self.line.pack(side='left', fill='y', expand=False, before=self.widget)
        self.line.bindtags((str(self.line), str(window.TKroot), "all"))
        self.line.tag_add("right", '1.0', "end")
        self.line.tag_configure("right", justify='right')
        self.line.delete('1.0', 'end')
        self.line.insert('1.0', '1', 'right')
        self.bind('<Configure>', '')
        self.bind('<MouseWheel>', '')
        self.vsb.configure(command=self.y_scroll)
        window.refresh()
        window.move_to_center()

    def reset(self):
        self.window.refresh()
        new_ratio, _ = self.vsb.get()
        new_lines = int(self.widget.index(sg.tk.END).split('.')[0]) - 1
        if new_lines != self.lines:
            self.lines = new_lines
            text = '\n'.join([f'{i+1}' for i in range(self.lines)])
            self.line.delete('1.0', 'end')
            self.line.insert('1.0', text)
            self.line.tag_add("right", '1.0', "end")
        if new_ratio != self.ratio:
            self.ratio = new_ratio
            self.line.yview_moveto(self.ratio)

    def y_scroll(self, action, n, what=None):
        if action == sg.tk.MOVETO:
            self.widget.yview_moveto(n)
            self.line.yview_moveto(n)
        elif action == sg.tk.SCROLL:
            self.widget.yview_scroll(n, what)
            self.line.yview_scroll(n, what)




# `application_active` flag used when updating the window after the first read, and after a theme change
# this is currently used as a work-around for the `finalize=True` bug, and can be
# removed when that issue is fixed
application_active = False 


##-----CREATE CUSTOM RE-DIRECT STDOUT OBJECT-------------##

class RedirectText:
    def __init__(self, window):
        ''' constructor '''
        self.window = window
        self.saveout = sys.stdout

    def write(self, string):
        self.window['_OUT_'].Widget.insert("end", string)

    def flush(self):
        sys.stdout = self.saveout 
        sys.stdout.flush()

##-----SETUP DEFAULT USER SETTINGS-----------------------##

save_user_settings = False
# if flag is `False` the settings are saved in active session only with a `dict`
# if flag is `True` the settings are saved in a local directory in a shelve file
if save_user_settings:
    import shelve
    settings = shelve.open('app_settings')
else:
    settings = {}

# check to see if settings dict/shelf contains default values, create if not
if len(settings.keys()) == 0:
    settings['theme'] = 'Dark'
    settings['themes'] = sg.list_of_look_and_feel_values()
    settings['font'] = ('Consolas', 12)
    settings['tabsize'] = 4
    settings['filename'] = './tmp.fuz'
    settings['body'] = ''
    settings['info'] = '> New File <'
    settings['out'] = ''
    settings['wdir'] = os.getcwd()

# default theme or user saved theme
sg.change_look_and_feel(settings['theme'])

# string to output initial start settings
outstring = "STARTUP SETTINGS:\n"+"-"*40+"\nTheme"+"."*10+" {}\nTab size"+"."*7+" {}\nFont"+"."*11+" {} {}\nOpen file"+"."*6+" {}\n\n"
settings.update(out = outstring.format(settings['theme'], settings['tabsize'], settings['font'][0], settings['font'][1], settings['filename']))

def close_settings():
    ''' Close the the shelve file upon exit '''
    settings.update(filename=None, body='', out='', info='> New File <')
    if save_user_settings:
        settings.close()


##----SETUP GUI WINDOW-----------------------------------##
#['Appearance',['Theme', settings['themes'],'Font','Tab Size','Show Settings']]
def main_window(settings):
    ''' Create the main window; also called when the application theme is changed '''
    elem_width= 90 # adjust default width
   

    # column1 = [
    #     [Multiline('',   size=(40, 10), justification='left', focus=True, pad=(0, 0),
    #     background_color='#404040', text_color='white', expand_x=True,
    #     expand_y=True, enable_events=True, horizontal_scroll=True, key='-ML-')],
    # ]
    # layout = [[sg.Column(column1, expand_x=True, expand_y=True, pad=(0, 0))]]
    # location = sg.Window.get_screen_size()
    # window = sg.Window('Title', layout, resizable=True, margins=(0, 0), location=location, finalize=True)
    # ml = window['-ML-']
    # ml.initial(window, width=5, bg='#202020', fg='#808080', font=font)









    menu_layout = [
        ['&File',['New','Open','Save','Save As','---','Exit']],
        ['&Edit',['Undo','---','Cut','Copy','Paste','Delete','---','Find...','Replace...','---','Select All']],
        ['&Insert',['ProgST','---','Trapez','Triangel','Bell','Sigmoid','---','Output','---','Statement','---','Date/Time']],
        ['Compile',['Compile Module']],
        ['&Help',['View Help','---','About FuNNy']]]
#
    col1 = sg.Column([[Multiline('', size=(60, 40),   justification='left', focus=True, pad=(0, 0),
        background_color='#404040', text_color='white', expand_x=True,
        expand_y=True, enable_events=True, key='_BODY_',right_click_menu=['&Right', ['Show Graph', 'Edit']])]])
    #col1 = sg.Column([[Multiline(default_text=settings['body'], font=settings['font'], key='_BODY_', size=(elem_width,40), enable_events=True,right_click_menu=['&Right', ['Show Graph', 'Edit']])]])
    col2 = sg.Column([[sg.Multiline(default_text=settings['out'], font=settings['font'], expand_x=True,
        expand_y=True, enable_events=True,  key='_OUT_', autoscroll=True, size=(elem_width,16))]])         
    #col1ny= sg.Column(column1, expand_x=True, expand_y=True, pad=(0, 0))
    window_layout = [
        [sg.Menu(menu_layout)],
        [sg.HorizontalSeparator()],
        [sg.Text(settings['info'], key='_INFO_', font=('Consolas',11), size=(elem_width,1), )],
        [sg.Pane([col1, col2],)]]

    window = sg.Window('FuNNy IDE V1.0 (PPM at CS:AAU)', window_layout, resizable=True, margins=(0,0), return_keyboard_events=True, finalize=True)
 
    #window.maximize()
    #window['_BODY_'].expand(expand_x=True,expand_y=True)
    return window


##----FILE MENU FUNCTIONS--------------------------------##

def new_file(window): # CTRL+N shortcut key
    ''' Setup a new session by clearing application variables and the body '''
    window['_BODY_'].update(value='')
    ml.reset()
    window['_INFO_'].update(value='> New File <')
    settings.update(filename='tmp.fuz', body='', info='> New File <')

def open_file(window): # CTRL+O shortcut key
    ''' Open a local file in the editor '''
    try: # 'OUT OF INDEX' error in trinket if 'CANCEL' button is pressed
        filename = sg.popup_get_file('File Name:', title='Open', no_window=True)
    except:
        return
    if filename not in (None,''):
        with open(filename,'r') as f:
            file_text = f.read()
        window['_BODY_'].update(value=file_text)
        ml.reset()
        window['_INFO_'].update(value=filename.replace('/',' > '))
        settings.update(filename=filename, body=file_text, info=filename.replace('/',' > '))
        os.chdir(os.path.dirname(filename))
        #window('_OUT_').set_vscroll_position(1.0)
        print(f"Open file...... {settings['filename']}")

def save_file(window, values): # CTRL+S shortcut key
    ''' Save active file. If new, then calls the `save_file_as` function '''
    filename = settings.get('filename')
    print(filename)
   
    if filename not in (None,''):
       
        with open(filename,'w') as f:
            f.write(values['_BODY_'])
        window['_INFO_'](value=filename.replace('/',' > '))
        settings.update(filename=filename, info=filename.replace('/',' > '))   
    else:
        save_file_as(window, values)
        os.chdir(os.path.dirname(filename))

def save_file_as(window, values):
    ''' Save the active file as another file, also called for new files '''
    try: # 'OUT OF INDEX' error in Trinket if 'CANCEL' button is clicked
        filename = sg.popup_get_file('Save File', save_as=True, no_window=True)
    except:
        return
    print(filename)

    if filename not in (None,''):
        with open(filename,'w') as f:
            f.write(values['_BODY_'])
        window['_INFO_'](value=filename.replace('/',' > '))
        settings.update(filename=filename, info=filename.replace('/',' > '))
        os.chdir(os.path.dirname(filename))


##----EDIT MENU FUNCTIONS--------------------------------##

def undo(window): # CTRL+Z shortcut key
    pass

def cut(window): # CTRL+X shortcut key
    try:
        selection= window['_BODY_'].Widget.get("sel.first", "sel.last")
        window.TKroot.clipboard_clear()
        window.TKroot.clipboard_append(selection)
        window['_BODY_'].Widget.delete("sel.first", "sel.last")
    except:
        return

def copy(window): # CTRL+C shortcut key
    try:
        selection= window['_BODY_'].Widget.get("sel.first", "sel.last")
        window.TKroot.clipboard_clear()
        window.TKroot.clipboard_append(selection)
        # try:
        #     selection = window['_BODY_'].Widget.selection_get()
        # except:
        #     return
    except:
        return

def Show_Graph(window):
    try:
        selection = window['_BODY_'].Widget.selection_get()
    except:
        return
    window['_OUT_'].update('')
    #window['_OUT_'].show()
    lex.plotMember(selection)


def paste(window): # CTRL+V shortcut key
    ''' paste clipboard contents into current cursor position '''
    try:
        clip = window.TKroot.clipboard_get()
    except:
        return
    else:
        window['_BODY_'].Widget.insert("insert", clip)



def delete(window):
    window['_BODY_'].Widget.delete("sel.first", "sel.last")
 

def find(window): # CTRL+F shortcut key
    pass

def replace(window): # CTRL+H shortcut key
    pass

def select_all(window): # CTRL+A shortcut key
    ''' select all text in the body '''
    window['_BODY_'].Widget.tag_add("sel","1.0","end")

def fetch_datetime(window, values):
    ''' Append the current date and time into the body '''
    datetime_stamp = datetime.now().strftime("%T %D")
    new_body = values['_BODY_'] + datetime_stamp
    window['_BODY_'].Widget.insert("insert",'# '+datetime_stamp)   
    settings.update(body=new_body)    


##----FORMAT MENU FUNCTIONS------------------------------##

def change_theme(window, event, values):
    ''' Change the color theme of the application window. This will destroy the active window and 
        recreate it with the same values.'''
    #old_theme = settings['theme']
    #if not old_theme == event:
    #    print(f"Theme.......... old_theme => event\n")
    settings.update(theme=event, body=values['_BODY_'], out=values['_OUT_'])
    sg.change_look_and_feel(event)
    window.close()

def change_font(window):
    ''' Change default font on body element and save as user settings '''
    # get the default font from user settings
    font_name, font_size = settings.get('font')
    # get available fonts from local environment to use in combo box
    font_list = sorted([f for f in tkfont.families() if f[0]!='@'])
    # check if default font is in font_list, if not return set first font in the list as default
    if not font_name in font_list:
      font_name = font_list[0]
    # available sizes to use for combo box (restricted to practical sizes)
    font_sizes = [8,9,10,11,12,14]
    # setup the font gui window
    font_layout = [
        [sg.Combo(font_list, key='_FONT_', default_value=font_name), 
         sg.Combo(font_sizes, key='_SIZE_', default_value=font_size)],[sg.OK(), sg.Cancel()]]
    font_window = sg.Window('Font', font_layout, keep_on_top=True)
    # listen for font selection events
    font_event, font_values = font_window.read()
    if font_event not in (None,'Exit'):
        font_selection = (font_values['_FONT_'], font_values['_SIZE_'])
        # check to see if the font changed
        if font_selection != settings['font']:
            settings.update(font=font_selection)
            window['_BODY_'].update(font=font_selection)
            window['_OUT_'].update(font=font_selection)
            print(f"Font........... {(font_name, font_size)} => {font_selection}\n")
    font_window.close()

def change_tabsize(window):
    ''' Change the tab size in the body. This is the user interface for the set_tabsize function '''
    tab_layout = [[sg.Slider(range=(1,8), default_value=settings['tabsize'], orientation='horizontal', key='_SIZE_'), sg.OK(size=(5,2))]]
    tab_window = sg.Window('Tab Size', tab_layout, keep_on_top=True)
    tab_event, tab_values = tab_window.read()
    if tab_event not in (None, 'Exit'):
        old_tab_size = settings['tabsize']
        new_tab_size = int(tab_values['_SIZE_'])
        # check to see if tab size changed
        if new_tab_size != old_tab_size:
            settings.update(tabsize=new_tab_size)
            set_tabsize(window, new_tab_size)
            print(f"Tab size....... old_tab_size => new_tab_size\n")
    tab_window.close()

def set_tabsize(window, size=4): # load upon opening after 'finalize=True' is fixed
    ''' Adjust the tab size in the body; default is 4 '''
    font = tkfont.Font(font=settings.get('font'))
    tab_width = font.measure(' '*size)
    window['_BODY_'].Widget.configure(tabs=(tab_width,)) 
    settings.update(tabsize=size) 

def show_settings():
    ''' Print the saved user settings to the Output element '''
    print(f"Theme.......... {settings['theme']}")
    print(f"Tab size....... {settings['tabsize']}")
    print( "Font.............. {}, {}".format(*settings['font']))
    print(f"Open file...... {settings['filename']}\n")


##----RUN MENU FUNCTIONS---------------------------------##

def Compile_Module(values): # F5 shortcut key
    ''' Run any python code that is in the body '''
    window['_OUT_'].update('')
    print('-'*50)
    print(f"Compiling: {settings['info']}")
    print('-'*50)
    filename = settings.get('filename')
    try:
        comFil= wpath+"/fuzzy"
        #print(comFil)
        retLs = subprocess.run(["funny", filename],stdout=subprocess.PIPE)
        #print(type.list_files)
        if len(retLs.stdout):
            print(retLs.stdout)
        else:
            print('Congratulations! No Error\n')
            #window['_OUT_'].update(fg='#808000')

            print('''The files:
    fuzout.c
    tstapp.c
    matlabapp.c 
    CMakeLists.txt
has been generated and are placed in the src directory
Build and run the tstapp:
    --$ cmake . 
    --$ make 
    --$ ./tstapp ''')
        
    except:
        print('ERROR!......... Invalid Python Code')
    


##----HELP MENU FUNCTIONS--------------------------------##

def get_help():
    msg = '''
    - Type, your FuNNy program, into the editor. 
    - Compile the program by using the Compile menu item. 
    - The C-code will be placed in: <Working dir>/src
    ----------
    Per P. Madsen: ppm@cs.aau.dk
    '''
    sg.Popup(msg,title='help')

def about_funny():
    msg = '''
    FunnyIDE is a simple IDE for the FuNNy language.

    It will automatically, 
    when compiling the FuNNy source kode from the editor, 
    generate alle the basic code e.g:
     - C-code for the FuNNy inferense machine, 
     - Test programs in C 
     - CMake file for compiling the C code.

    Version: {}
    Date:  {}
    '''.format(VERSION, V_DATE)
    sg.popup(msg,title='About the FuNNy IDE')

def ins_ProgST(window):
    window['_BODY_'].Widget.insert("insert", '''# input section
    X: input;
    Y: input;

# Membership functions

    HIGH X: trapez(25.0,a, 30.0,a, 100,a, 100,a, 1.0,c);
    MID  X: triangle(18,a, 25,a, 30,a, 1.0,c);
  	LOW  X: trapez(0,a, 0,a, 18,a, 25.0,a, 1.0,c);
 

    HIGH Y: sigmoid(30.0,a, 25.0,a, 1.0,c);
    MID  Y: triangle(18,a, 25,a, 30,a, 1.0,c);
  	LOW  Y: sigmoid(15.0,a, 25.0,a, 1.0,c);

    UP   OUT1: output( 2.0,a);
    ZERO OUT1: output( 0.0,a);
    DOWN OUT1: output(-2.0,a);
 
 	UP   OUT2: output( 2.0,a);
    ZERO OUT2: output( 0.0,a);
    DOWN OUT2: output(-2.0,a);
 

rules

# Rule section    

	if X is HIGH and Y is LOW      then OUT1 is UP
    if X is LOW  and Y is MID      then OUT1 is ZERO
    if X is LOW  and Y is LOW      then OUT1 is DOWN

	if X is not extremely HIGH or  Y is LOW  then OUT2 is UP
    if X is more-or-less  LOW  or  Y is not MID  then OUT2 is ZERO
    if X is LOW and Y is very LOW and Y is about 22.0 to about 24.0 then OUT2 is DOWN''')
    ml.reset()

def ins_Trapez(window):
    window['_BODY_'].Widget.insert("insert", '    HIGH X: trapez(1.0,a, 1.2,a, 1.8,a, 2.0,a, 1.0,c);\n')
    ml.reset()

def ins_Triangel(window):
    window['_BODY_'].Widget.insert("insert", '    HIGH X: triangle(1.0,a, 1.5,a, 2.0,a, 1.0,c);\n')
    ml.reset()

def ins_Bell(window):
    window['_BODY_'].Widget.insert("insert", '    HIGH X: bell(1.0,a, 0.2,a, 1.0,c);\n')
    ml.reset()

def ins_Sigmoid(window):
    window['_BODY_'].Widget.insert("insert", '    HIGH X: sigmoid(1.0,a, 2.0,a, 1.0,c);\n')
    ml.reset()

def ins_Rule(window):
    window['_BODY_'].Widget.insert("insert", '    if X is HIGH and Y is LOW then OUT is LOW\n')
    ml.reset()

def ins_Output(window):
    window['_BODY_'].Widget.insert("insert", '    LOW OUT: output(2.0,a);\n')
    ml.reset()



##----MAIN EVENT LOOP------------------------------------##



font = ('Courier New', 12)
sg.theme('Darkblue')
sg.set_options(font=font)

wpath = os.path.dirname(os.path.abspath(__file__))
print(wpath)
window = main_window(settings)
ml = window['_BODY_']
ml.initial(window, width=5, bg='#202020', fg='#808080', font=font)
redir = RedirectText(window)
sys.stdout = redir
#'ProgST','---','Trapez','Triangel','Bell','Sigmoid','---','Output','---','Statement'
if not os.path.exists('./src'):
    os.mkdir('src')
os.chdir('src')
while True:
    event, values = window.read(timeout=1)

    # adjust window when application is activated
    if not application_active:
        application_active = True
        set_tabsize(window)
   
    # listen for window events

    if event in (None, 'Exit'):
        close_settings()
        break
    if event in ('New','n:78'):
        new_file(window)
    if event in ('Open','o:79'):
        open_file(window)
    if event in ('Save','s:83'):
        save_file(window, values)
    if event in ('Save As',):
        save_file_as(window, values)
    if event in ('Date/Time',):
        fetch_datetime(window, values)
    if event in ('ProgST',):
        ins_ProgST(window)
    if event in ('Trapez',):
        ins_Trapez(window)
    if event in ('Triangel',):
        ins_Triangel(window)
    if event in ('Bell',):
        ins_Bell(window)
    if event in ('Sigmoid',):
        ins_Sigmoid(window)
    if event in ('Output',):
        ins_Output(window)
    if event in ('Statement',):
        ins_Rule(window)
    if event in ('Font',):
        change_font(window)
    if event in ('Tab Size',):
        change_tabsize(window)
    if event in ('Show Settings',):
        show_settings()
    if event in ('Compile Module', 'F5:116' ):
        save_file(window, values)
        Compile_Module(values)
    if event in ('Compile',):
        save_file(window, values)
        Compile_Module(values)
    if event in settings['themes']: 
        ###############################>>> refactor this bit into a function
        # set application to inactive
        application_active = False
        change_theme(window, event, values)
        # recreate window and redirect object
        sys.stdout = redir.saveout
        window = main_window(settings)
        redir = RedirectText(window)
        sys.stdout = redir
    if event in ('View Help',):
        get_help()
    if event in ('About FuNNy',):
        about_funny()
    if event in ('Show Graph',):
        Show_Graph(window)
    if event in ('Paste',):
        paste(window)
    if event in ('Cut',):
        cut(window)
    if event in ('Copy',):
        copy(window)
    if event in ('Delete',):
        delete(window)
    if event in ('Select All',):
        select_all(window)
    if event == sg.WIN_CLOSED:
        break
    elif event == '_BODY_':
        ml.reset()

