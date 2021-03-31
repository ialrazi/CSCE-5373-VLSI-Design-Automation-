import os
import sys
import glob
from PySide import QtCore, QtGui
from PySide.QtGui import QFileDialog
import matplotlib.pyplot as plt
from matplotlib.figure import Figure
from matplotlib.backends.backend_qt4agg import FigureCanvasQTAgg as FigureCanvas
from Simulated_Annealing_ui import Ui_MainWindow
from PIL import Image
from PIL import ImageQt
from scipy.interpolate import interp1d
from subprocess import call
import numpy as np

class saTool(QtGui.QMainWindow):
    def __init__(self):
        QtGui.QMainWindow.__init__(self, None)
        self.init()

    def init(self):
        # set up main window GUI
        self.ui = Ui_MainWindow()
        self.ui.setupUi(self)
        self.best_file_key=None
        self.ui.btn_inp_file.pressed.connect(self.open_dir) # choose file button
        self.ui.btn_ok.pressed.connect(self.collect_data) # start SA button
        self.ui.btn_plt_out.pressed.connect(self.readFile) # output plot button
        self.ui.btn_best.pressed.connect(self.plot_best) # best solution button
        self.ui.btn_plt_out.setEnabled(False) # disable output plot button until SA finishes


        # showing figure in canvas
        fig1 = Figure()
        self.ax1 = fig1.add_subplot(111)
        self.canvas_sol_browser = FigureCanvas(fig1)
        #grid_layout = QtGui.QGridLayout(self.ui.graph_view_layout)
        self.scene = QtGui.QGraphicsScene()
        self.ui.graph_view_layout.setScene(self.scene)
        grid_layout = QtGui.QGridLayout(self.ui.graph_view_sols)
        grid_layout.setContentsMargins(0, 0, 0, 0)
        grid_layout.addWidget(self.canvas_sol_browser, 0, 0, 1, 1)


    # reads the output file to plot the floorplans and corresponding properties (i.e. area, time, utilization, aspect ratio)
    def readFile(self):
        with open(self.file_loc) as f:
            content = f.read().splitlines()

        Info=[] # storing information in list
        for i in content:
            if '#' in i:  # ignoring lines with #
                continue
            else:
                c=i.split()
                if len(c)>0:
                    Info.append(c)

        self.sol_browse_info={} # stores output information for showing in the GUI
        self.best=None
        for i in Info:
            if len(i)>2:
                key=(float(i[1]),float(i[2])) # (x,y)=(temperature, area)
                value=[]
                value.append(i[3]) # floorplan output image location
                value.append(int(i[0])) #index number
                value.append(int(i[6])) # time
                value.append(float(i[4])) # aspect ratio
                value.append(i[5]) # utilization
                value.append(float(i[2])) # area
                self.sol_browse_info[key]=value
            elif len(i)==2:
                self.best=int(i[1]) # index number of best solution

        self.update_sol_browser()



    def open_dir(self): # responds to button click by opening a file browser where the project directory can be selected
        prev_folder = 'C://'
        '''
        if not os.path.exists(prev_folder):  # check if the last entry file store a correct path
            prev_folder = 'C://'
        '''
        directory = QFileDialog.getOpenFileName(self, "Select file",self.ui.lin_cwd.text(), "Script Files (*.fp)")
        self.input_file=directory[0]
        index_of_dot = self.input_file.rindex('.')  # finding the index of (.) in path
        index_of_slash=self.input_file.rindex('/benchmarks')
        self.parent = self.input_file[:index_of_slash]
        self.input_base = os.path.basename(self.input_file[:index_of_dot])  # extracting basename from path

        self.inp_file_dir = os.path.dirname(os.path.abspath(self.input_file))

        self.ui.lin_cwd.setText(self.input_file)
        return

    # calls C++ functions to read the input file(.fp) and build module data base and HCG,VCG
    def collect_data(self):
        os.system("Parser\\parser.exe "+self.input_file) # calling c++ file
        # SA parameters (reading from GUI)
        self.initialTemp=float(self.ui.lin_temp.text())  # initial temperature
        self.alpha=float(self.ui.lin_alpha.text()) # alpha
        self.beta=float(self.ui.lin_beta.text()) # beta
        self.total_time=int(self.ui.lin_duration.text()) # total time
        self.label_print=0 # if user wants to see the module labels then label_print should be 1
        if self.ui.check_bx_show_label.isChecked():
            self.label_print=1

        # writing all necessary information in a file for passing to simulated annealing function (furnace.exe)
        self.spg_file=self.parent+"\\outputs\\parsed.spg"
        f = open("outputs\\inputs.cfg", "w")
        f.write("%s" % self.spg_file)
        f.write("\n")
        f.write("%s" %self.initialTemp)
        f.write("\n")
        f.write("%s" %self.alpha)
        f.write("\n")
        f.write("%s" %self.beta)
        f.write("\n")
        f.write("%s" %self.total_time)
        f.write("\n")
        f.write("%s" % self.label_print)
        f.close()
        self.call_SA()

    def call_SA(self):
        os.chdir("furnace")
        os.system("furnace.exe ../outputs/inputs.cfg")
        os.chdir("..")
        self.file_loc = self.parent+"/outputs/output.plt"
        if os.path.isfile(self.file_loc):
            os.chdir("outputs")
            files = glob.glob("*.gnu");
            print files
            for f in files:
                os.system("gnuplot " + f);
            os.chdir("..")
            self.ui.btn_plt_out.setEnabled(True)

    def showOutput(self):
        self.ui.lin_area.setText(str(self.area))
        self.ui.lin_ar_rtio.setText(str(self.aspect_ratio))
        self.ui.lin_utilize.setText(str(self.utilization))
        self.ui.lin_iter_num.setText(str(self.time))

    def update_sol_browser(self):
        self.ax1.clear()
        print "plot sol browser"
        self.x_axis = {"label": "Temperature", 'data': []}
        self.y_axis = {"label":"Area", 'data': []}
        Keys=self.sol_browse_info.keys()
        Keys.sort()
        for k in Keys:
            self.x_axis['data'].append(k[0])
            self.y_axis['data'].append(k[1])
        f = interp1d(self.x_axis['data'], self.y_axis['data'])
        self.ax1.plot(self.x_axis['data'],self.y_axis['data'], 'o',self.x_axis['data'], f(self.x_axis['data']),  picker=5)
        self.ax1.set_xlabel(self.x_axis['label'])
        self.ax1.set_ylabel(self.y_axis['label'])
        self.ax1.ticklabel_format(style='sci', axis='y', scilimits=(0, 0))
        self.canvas_sol_browser.draw()
        self.canvas_sol_browser.callbacks.connect('pick_event', self.on_pick)

    def on_pick(self, event):
        self.update_sol_browser()
        ind = event.ind[0] 
        self.key=self.x_axis['data'][ind], self.y_axis['data'][ind]
        self.floorplanPlot()

    def plot_best(self):
        self.fig_file=None
        if self.best!=None:
            for k,v in self.sol_browse_info.items():
                print v[1]
                if v[1]==self.best:
                    self.key=k
                    
                    self.floorplanPlot()



    def floorplanPlot(self):
        self.fig_file = self.sol_browse_info[self.key][0]
        self.scene.clear()
        os.chdir("furnace")
        img = Image.open(self.fig_file)
        img = img.resize((525,475))
        self.imgQ = ImageQt.ImageQt(img)  
        pixMap = QtGui.QPixmap.fromImage(self.imgQ)
        self.scene.addPixmap(pixMap)
        self.scene.update()
        self.area=self.sol_browse_info[self.key][5]
        self.aspect_ratio=self.sol_browse_info[self.key][3]
        self.utilization=self.sol_browse_info[self.key][4]
        self.time=self.sol_browse_info[self.key][2]
        self.showOutput()
        os.chdir("..")
       


if __name__ == "__main__":
    os.system("del outputs\\*.* /Q /F")
    app = QtGui.QApplication(sys.argv)
    main_window = saTool()
    main_window.show()
    sys.exit(app.exec_())


