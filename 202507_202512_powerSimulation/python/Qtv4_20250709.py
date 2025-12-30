import math
import sys

from PyQt6.QtGui import QFont, QIntValidator, QDoubleValidator
from PyQt6.QtWidgets import QApplication, QMainWindow, QVBoxLayout, QWidget, QLabel, QMdiArea, QMdiSubWindow, QLineEdit, \
    QSlider, QDial
from PyQt6.QtCore import Qt, QTimer
from datetime import datetime
from pyqtgraph import PlotWidget, AxisItem
from qt_material import apply_stylesheet


# AxisItem 오버라이딩
class TimeAxisItem(AxisItem):
    def tickStrings(self, value, scale, spacing):
        labels = []
        for t in value:
            dt = datetime.fromtimestamp(t)
            label = dt.strftime('%H:%M:%S')
            labels.append(label)
        return labels


class MyApp(QMainWindow):
    def __init__(self):
        super().__init__()

        # 전압 사인파 출력
        self.phase = 0  # 초기 위상
        self.frequency = 1  # 주파수 (Hz)
        self.amplitude = 100  # 진폭 (최대 전압)
        self.time = 1

        # 전압값 초기 설정
        self.last_voltage = 100  # 초기 전압값
        self.last_intensity = 100 #초기 전류값
        self.vinputData = [] # 전압, 시간(현재 시간) 저장
        self.iinputData = []  # 전류, 시간(현재 시간) 저장

        # 타이머 초기설정
        self.timer = QTimer(self)  # <-- 타이머 객체 생성
        self.timer.timeout.connect(self.update_graph)  # 타이머 연결
        self.timer.start(100)
        self.start_time = datetime.now().timestamp()

        self.initUI()
        self.setWindowTitle("QMdiArea 예제")
        self.setGeometry(300,150,1920,1080)

    def initUI(self):
        self.mdi_area = QMdiArea() # 메인 창
        self.setCentralWidget(self.mdi_area)


        ########################## 첫번째 서브창(데이터 입력창) #############################

        sub1 = self.create_subwindow("데이터 입력", "") #서브창1
        self.mdi_area.addSubWindow(sub1)
        sub1.setGeometry(0,0, 960,1080)

        sub_widget = QWidget()
        sub1.setWidget(sub_widget)

        # 폰트설정
        font = QFont()
        font.setPointSize(30)
        font.setFamily("고딕")

        # 전압라벨
        self.Vlabel = QLabel("전압", sub_widget)
        self.Vlabel.setFont(font)
        self.Vlabel.move(250, 100)
        self.Vlabel.resize(100, 50)

        # 전압 입력필드
        self.VSlider = QSlider(Qt.Orientation.Horizontal, sub_widget)
        self.VSlider.move(350,130)
        self.VSlider.resize(200, 50)

        # V라벨
        self.Vlabel2 = QLabel("V", sub_widget)
        self.Vlabel2.setFont(font)
        self.Vlabel2.move(580, 100)
        self.Vlabel2.resize(100, 50)


        #전류라벨
        self.iLabel = QLabel("전류", sub_widget)
        self.iLabel.setFont(font)
        self.iLabel.resize(100, 50)
        self.iLabel.move(250, 200)

        # 전류 입력필드
        self.iSlider = QSlider(Qt.Orientation.Horizontal, sub_widget)
        self.iSlider.move(350, 230)
        self.iSlider.resize(200, 50)

        # A라벨
        self.ilabel2 = QLabel("A", sub_widget)
        self.ilabel2.setFont(font)
        self.ilabel2.move(580, 200)
        self.ilabel2.resize(100, 50)


        # 시간간격라벨
        self.Slabel = QLabel("시간", sub_widget)
        self.Slabel.setFont(font)
        self.Slabel.move(250, 300)
        self.Slabel.resize(100, 50)

        # 시간간격 입력필드
        self.sSlider = QSlider(Qt.Orientation.Horizontal, sub_widget)
        self.sSlider.move(350, 330)
        self.sSlider.resize(200, 50)

        # s라벨
        self.Slabel2 = QLabel("s", sub_widget)
        self.Slabel2.move(580, 300)
        self.Slabel2.resize(100, 50)

        #전압위상라벨
        self.pLabel = QLabel("위상", sub_widget)
        self.pLabel.setFont(font)
        self.pLabel.move(620, 80)
        self.pLabel.resize(50, 50)

        #위상 다이얼 전압용
        self.vDial = QDial(sub_widget)
        self.vDial.setRange(0,440)
        self.vDial.move(670, 80)
        self.vDial.resize(100, 100)

        # 전류위상라벨
        self.pILabel = QLabel("위상", sub_widget)
        self.pILabel.setFont(font)
        self.pILabel.move(620, 200)
        self.pILabel.resize(50, 50)

        # 위상 다이얼 전압용
        self.iDial = QDial(sub_widget)
        self.iDial.setRange(0, 440)
        self.iDial.move(670, 200)
        self.iDial.resize(100, 100)


        #주파수 라벨
        self.fLabel = QLabel("주파수", self)
        self.fLabel.setFont(font)
        self.fLabel.resize(100, 50)
        self.fLabel.move(250, 450)

        #주파수 입력 라벨
        self.fSlider = QSlider(Qt.Orientation.Horizontal, sub_widget)
        self.fSlider.move(350,450)
        self.fSlider.resize(200, 50)

        # Hz라벨
        self.flabel2 = QLabel("Hz", sub_widget)
        self.flabel2.move(580, 450)
        self.flabel2.resize(100, 50)


        # 전압, 시간 범위 지정
        self.VSlider.setMinimum(-500)
        self.VSlider.setMaximum(500)

        self.sSlider.setMinimum(1)
        self.sSlider.setMaximum(60)

        self.fSlider.setMinimum(1)
        self.fSlider.setMaximum(60)

        #엔터키 이벤트 (전압 혹은 시간을 눌렀을 때 시그널)
        self.VSlider.valueChanged.connect(self.Vhandle_input)
        self.iSlider.valueChanged.connect(self.ihandle_input)
        self.sSlider.valueChanged.connect(self.Shandle_input)

        sub1.show()

        #실시간 입력 값 받기(엔터키 이벤트)

        ##################################두번째 서브창(그래프 출력)#####################################################

        sub2 = self.create_subwindow("그래프 출력", "") #서브창2
        self.mdi_area.addSubWindow(sub2)
        sub2.setGeometry(960,0, 960, 1080)

        # 서브창 안에 그래프를 담을 위젯 준비
        graph_widget = QWidget()

        # pyqtgraph 위젯 생성 및 설정
        self.plot_widget1 = PlotWidget(graph_widget, axisItems = {'bottom' : TimeAxisItem(orientation='bottom')})
        self.plot_widget1.setFixedSize(800, 400)  # 크기 설정
        self.plot_widget1.move(50,50)
        self.plot_widget1.setBackground('w')  # 배경 흰색
        self.plot_widget1.showGrid(x=False, y=False)  # 격자 표시
        self.plot_widget1.setLabel('left', '전압 (V)')
        self.plot_widget1.setLabel('bottom', '시간 (s)')

        self.plot_widget2 = PlotWidget(graph_widget, axisItems={'bottom': TimeAxisItem(orientation='bottom')})
        self.plot_widget2.setFixedSize(800, 400)  # 크기 설정
        self.plot_widget2.move(50, 450)
        self.plot_widget2.setBackground('w')  # 배경 흰색
        self.plot_widget2.showGrid(x=False, y=False)  # 격자 표시
        self.plot_widget2.setLabel('left', '전류 (A)')
        self.plot_widget2.setLabel('bottom', '시간 (s)')

        sub2.setWidget(graph_widget)

        sub2.show()


    #서브창 생성 함수
    def create_subwindow(self, title, text):
        sub_window = QMdiSubWindow() #서브창 생성

        sub_window.setWindowTitle(title)  # 타이틀 제목
       # sub_window.setWindowFlags(Qt.WindowType.SubWindow)

        widget = QWidget()
        layout = QVBoxLayout()
        label = QLabel(text)
        layout.addWidget(label)
        widget.setLayout(layout)

        sub_window.setWidget(widget)
        return sub_window

    #전압 입력 시 슬롯
    def Vhandle_input(self):
        try:
            voltage = int(self.VSlider.value())
            self.last_voltage = voltage  # 전압 즉시 갱신
            print(f"새 전압값 설정됨: {voltage}V")
        except ValueError:
            print("전압 입력 오류")

    def ihandle_input(self):
        try:
            intensity = int(self.iSlider.value())
            self.last_intensity = intensity  # 전압 즉시 갱신
            print(f"새 전압값 설정됨: {intensity}V")
        except ValueError:
            print("전압 입력 오류")

    # 시간 입력시 슬롯 - 타이머 주기 업데이트 (시간 입력 창에서 엔터키 입력시)
    def Shandle_input(self):
        try:
            #타이머의 주기를 변경
            interval_sec = self.sSlider.value() / 10.0
            interval_ms = int( interval_sec * 1000)

            #타이머를 멈추고
            self.timer.stop()
            #다시 새로운 주기로 스타트
            self.timer.start(interval_ms)

            print(f"그래프 업데이트 주기 설정됨: { interval_sec}초 간격")
        except ValueError:
            print("시간 입력 오류: 실수 값을 입력하세요.")



    # 그래프 업데이트
    def update_graph(self):
        #voltage = self.last_voltage
        self.amplitude = self.last_voltage
        self.phase = math.radians(self.vDial.value())  # 위상 변화
        self.frequency = self.fSlider.value()
        current_time = datetime.now().timestamp() #현재 시간
        elapsed = current_time - self.start_time  # 현재시간 - 처음 시작한 시간
        self.time = elapsed

        # 시간에 따라 교류전압 생성
        voltage = self.amplitude * math.sin(2 * math.pi * self.frequency  * self.time + self.phase)

        # 현재 시간을 저장
        timestamp = datetime.now().timestamp()
        self.vinputData.append((voltage, timestamp))

        # 데이터 길이 조정(100)
        if len(self.vinputData) > 100:
            self.vinputData.pop(0)

        #x는 t만 가져옴, y는 v만 가져옴
        x = [t for v, t in self.vinputData]
        y = [v for v, t in self.vinputData]
        # 지우고 모든 입력을 새로 없데이트
        self.plot_widget1.clear()
        #선만 나타내게 하기
        self.plot_widget1.plot(x, y, pen='b')

         ###################전류그래프######################

        self.amplitude = self.last_intensity
        self.phase = math.radians(self.iDial.value())  # 위상 변화
        self.frequency = self.fSlider.value()
        current_time = datetime.now().timestamp()  # 현재 시간
        elapsed = current_time - self.start_time  # 현재시간 - 처음 시작한 시간
        self.time = elapsed

        # 시간에 따라 교류전압 생성
        intensity = self.amplitude * math.sin(2 * math.pi * self.frequency * self.time + self.phase)

        # 저장
        self.iinputData.append((intensity, timestamp))

        # 데이터 길이 조정(100)
        if len(self.iinputData) > 100:
            self.iinputData.pop(0)

        # x는 t만 가져옴, y는 v만 가져옴
        x = [t for v, t in self.iinputData]
        y = [v for v, t in self.iinputData]
        # 지우고 모든 입력을 새로 없데이트
        self.plot_widget2.clear()
        # 선만 나타내게 하기
        self.plot_widget2.plot(x, y, pen='b')


if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MyApp()
    apply_stylesheet(app, theme='dark_cyan.xml')
    window.show()
    sys.exit(app.exec())