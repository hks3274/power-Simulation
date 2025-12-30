# power simulation
**[구조 문서화]**

### 1. 개요
- 본 프로젝트는 Qt를 활용해 전력 계측 시뮬레이션 소프트웨어를 직접 설계하고 구현하는 것을 목표로 진행되었다. 전력 계측은 실제 산업 현장에서 전압·전류 변화나 전력 품질, 고조파 같은 여러 전기적 요소를 분석하는 데 핵심 역할을 한다.
- 이번 프로젝트에서는 이러한 계측 장비의 주요 기능을 소프트웨어로 재구성하여, 전기 신호를 직관적으로 확인하고 분석할 수 있는 데스크톱 기반 시뮬레이션 환경을 만드는 데 중점을 두었다. 이를 통해 실제 장비 없이도 전력 신호의 특성을 이해하고 실험할 수 있는 구조를 갖추게 되었다.
- 이 문서는 프로젝트의 전체 구조를 클래스 다이어그램과 시퀀스 다이어그램을 통해 정리한 자료이다. 각 구성 요소가 어떤 역할을 수행하는지, 그리고 모듈 간 데이터 흐름이 어떻게 이루어지는지를 두 다이어그램을 기반으로 명확하게 설명하였다. 이를 통해 프로젝트의 아키텍처와 동작 과정을 한눈에 파악할 수 있도록 구성하였다.

### 2. 클래스 구조 
<p align="center">
      <img src="https://github.com/hks3274/power-Simulation/blob/main/diagram_images/power_simulation_%ED%81%B4%EB%9E%98%EC%8A%A4%EB%8B%A4%EC%9D%B4%EC%96%B4%EA%B7%B8%EB%9E%A8_%EC%9A%94%EC%95%BD.PNG?raw=true"width="70%">
</p>

- 위 그림은 본 프로젝트에서 핵심이 되는 구성 요소들을 클래스 다이어그램 형태로 정리한 것이다.

#### 1. datamanagement
<p align="center">
      <img src="https://github.com/hks3274/power-Simulation/blob/main/diagram_images/power_simulation_%ED%81%B4%EB%9E%98%EC%8A%A4_%20dataMng.PNG?raw=true" width="50%">
</p>


> - 프로젝트 **전체 데이터 흐름을 담당**하는 핵심 클래스이다. 모든 주요 클래스는 datamanagement 객체의 참조를 받아 내부 데이터를 활용하며, 이 클래스가 전체 시스템의 중심 역할을 수행한다.

- 다양한 데이터 변수에 접근할 수 있도록 `getter`와 `setter`를 제공하여 다른 클래스들이 `datamanagement`의 값을 안전하게 읽고 수정할 수 있도록 한다.

- 내부에 `QChronoTimer`를 포함하고 있어 샘플 단위와 주기 단위를 모두 제어하는 역할을 수행한다. 매 샘플마다 `dataChanged()` 시그널을 발생시켜 실시간 파형 업데이트를 요청하고, 한 주기가 완료될 때는 `rmsDataChanged()` 시그널을 전달하여 RMS 계산 및 Phasor 업데이트 등 주기 기반 처리가 필요한 클래스들과 직접적으로 연동된다.

- 클래스들 간의 관계는 **약한 포함(aggregation) 관계**에 해당하며, 데이터의 중심 역할을 하는 `dataManagement`가 소멸하더라도 이를 참조하는 다른 클래스들이 함께 사라지지는 않는다. 즉, 각 클래스는 독립적으로 존재할 수 있으며 `dataManagement`와는 느슨하게 연결된 구조를 가진다.

#### 2. 입력(InputMainWindow)
<p align="center">
      <img src="https://github.com/hks3274/power-Simulation/blob/main/diagram_images/power_simulation_%ED%81%B4%EB%9E%98%EC%8A%A4_input.PNG?raw=true" width="50%">
</p>

>- **입력 UI를 담당**하는 핵심 클래스들이며, 사용자가 전압, 전류, 고조파 등 다양한 파라미터를 직접 설정하는 역할을 수행한다. 이곳에서 사용자가 변경한 모든 값은 즉시 `dataManagement`로 전달되어 전체 시뮬레이션 과정에 반영된다. 또한 각 입력 위젯은 **QDockWidget** 형태로 `inputMainWindow`에 존재해, 사용자가 원하는 위치로 자유롭게 배치하거나 분리할 수 있도록 설계하였다. 

- `inputWidget`은 **전압과 전류의 3상(A·B·C상) 값과 각도(phase)** 를 입력받는 역할을 수행하며, 시뮬레이션의 기본 파형을 정의하는 가장 핵심적인 UI 요소이다

- `inputSettingWidget`은 시뮬레이션의 **전반적인 동작 환경을 결정하는 설정 값**을 입력받는다. 여기에는 주파수, 초당 사이클 수(cycle per second), 사이클당 샘플 수(sample per cycle) 등이 포함되며, 이는 샘플링 해상도와 파형 생성 속도 같은 시스템의 타이밍 구조에 직접적인 영향을 미친다.

- `harmonicWidget`은 기본 파형에 추가되는 **고조파(harmonics)의 크기와 각도를 설정**하는 UI로, 고조파가 전력에 미치는 영향을 실험할 때 중요한 역할을 한다. 특정 차수의 고조파를 선택해 크기와 위상을 조정할 수 있다.

- `inputWidget`은 내부에 `sliderEdit`과 `dialEdit`과 같은 반복적으로 사용되는 입력 그룹을 별도 클래스로 묶어 재사용성 있게 구성하였다. 이를 통해 UI 요소를 일관된 방식으로 관리할 수 있을 뿐만 아니라, 동일한 입력 패턴을 여러 곳에서 사용할 때 발생하는 코드 중복을 크게 줄였다. 

- `inputWidget`, `inputSettingWidget`, `harmonicWidget`은 모두 `inputMainWindow` 내부에 포함된 구성 요소로, `inputMainWindow`가 소멸하면 함께 사라지는 **강한 포함(composition) 관계**를 가진다. 각각의 위젯이 실제로 사용자 입력을 받아 `dataManagement`에 값을 전달하는 역할을 수행하지만, 이 위젯들은 독립적으로 존재하는 것이 아니라 `inputMainWindow`의 UI 구조와 생명주기에 완전히 종속된 형태로 동작한다. 즉, 입력 처리의 책임은 각 위젯이 담당하되, 이들을 하나의 입력 인터페이스로 묶어 관리하는 중심 컨테이너가 `inputMainWindow`라고 볼 수 있다. 
- `sliderEdit`과 `dialEdit` 역시 `inputWidget` 내부에서 생성되고 관리되는 **강한 포함(composition) 관계**를 가진다. 즉, `inputWidget`이 소멸하면 이들 컴포넌트도 함께 소멸관계로 구성되지만, 구조적으로는 범용 입력 컴포넌트로 설계되어 있기 때문에 필요하다면 다른 위젯에서도 동일한 방식으로 재사용이 가능하다. 

#### 3. 출력(MainWindow-output)
<p align="center">
      <img src="https://github.com/hks3274/power-Simulation/blob/main/diagram_images/power_simulation_%ED%81%B4%EB%9E%98%EC%8A%A4_output.PNG?raw=true" width="50%">
</p>

>- **출력 화면을 담당**하는 핵심 클래스들로, 사용자가 실제로 확인하는 그래프·수치·파형 등의 UI 요소를 구성한다. 이 클래스들은 `dataManagement`로부터 전달된 데이터를 받아 필요한 형태로 가공한 뒤 화면에 표시하는 역할을 수행한다. 또한 각 출력 위젯은 `MainWindow` 내부에서 **QDockWidget** 형태로 배치되므로, 사용자가 창의 위치를 자유롭게 이동하거나 분리하여 사용할 수 있다.

- `graphWidget`은 시뮬레이션된 3상 전압, 전류 합성파를 매 샘플 주기마다 실시간으로 그래프로 표시하는 역할을 수행한다. `dataManagement`에서 발생하는 `dataChanged()` 시그널이 바로 한 샘플 주기마다 발생하는 신호이기 때문에, graphWidget은 이 시그널을 수신할 때마다 최신 파형 데이터를 받아와 **QCustomPlot**을 통해 화면에 즉시 갱신한다. 이를 통해 사용자는 합성된 전압·전류 파형을 확인할 수 있다.

- `RMSGraphWidget`은 파형의 한 주기가 완료될 때마다 RMS 값을 계산하여 그래프로 시각화하는 역할을 담당한다. `dataManagement`는 한 주기가 끝날 때마다 `rmsDataChanged()` 시그널을 발생시키며, `RMSGraphWidget`은 이 시그널을 수신하여 최신 RMS 데이터를 받아온다. 이를 바탕으로 3상 전압·전류의 RMS, 고조파 RMS, 그리고 고조파가 포함된 3상 합성 RMS 파형까지 함께 그래프로 표시한다. 이 그래프 또한 **QCustomPlot**을 사용해 화면에 갱신한다. 

- `phasorWidget` 역시 한 주기가 완료될 때마다 갱신되며, 각 상의 위상 및 크기를 Phasor 형태로 시각화하는 역할을 수행한다. 실제 계산된 Phasor 값은 별도의 `phasorCalculator` 클래스에서 제공되지만(요약 구조에서는 생략), `phasorWidget`은 `rmsDataChanged()` 시그널을 통해 주기별 최신 값을 받아와 **QPainter**을 사용해 화면에 표시하도록 설계되어 있다.

- `graphWidget`, `RMSGraphWidget`, `phasorWidget`은 모두 `MainWindow` 내부에 포함되어 있는 구성 요소로, `MainWindow`가 소멸하면 함께 사라지는 **강한 포함(composition) 관계**를 가진다. 각 위젯은 독립적으로 존재하지 않고 `MainWindow`의 생명주기에 종속되는 구조로 설계되어 있다.

#### 4. 1초 데이터창(oneSecMainWindow)
<p align="center">
      <img src="https://github.com/hks3274/power-Simulation/blob/main/diagram_images/power_simulation_%ED%81%B4%EB%9E%98%EC%8A%A4_oneSec.PNG?raw=true" width="50%">
</p>

>- `oneSecMainWindow`는 1초 단위로 누적되는 데이터를 관리하고 표시하는 핵심 클래스이다. 이 클래스는 `dataManagement`에서 전달되는 주기별 출력값을 기반으로, 여러 전력 계측 공식을 적용하여 추가적인 계측 항목을 계산한다. 이를 통해 사용자는 단순한 파형 정보뿐 아니라 전압·전류의 평균값, 유효전력, 무효전력, 역률 등 전력 분석에 필요한 다양한 지표를 한눈에 확인할 수 있도록 구성하였다.

- `oneSecMainWindow`는 직접 계산을 수행하지 않으며, 실제 데이터 가공은 `oneSecCalcData` 클래스에서 이루어진다. `oneSecCalcData`는 `dataManagement`로부터 전달되는 1초 누적 데이터를 다양한 계측 공식에 따라 처리한 뒤, **tableDat와 oneSecList를 생성**한다.

- 이 중 **tableData**는 `oneSecMainWindow`로 전달되어 테이블 UI를 구성하는 데 사용되며, 사용자는 이를 통해 1초 단위로 계산된 전압·전류·전력 등 다양한 계측 정보를 확인할 수 있다. 반면 **oneSecList**는 이후 설명할 `A3700N_GUI` 클래스에 전달되어 출력 페이지에서 활용된다.

- `oneSecCalcData` 클래스는 `dataManagement`로부터 한 주기마다 전달되는 시그널을 수신하여, 이를 누적해 1초 단위의 데이터를 계산하는 역할을 수행한다. 이렇게 1초 동안 모인 주기별 데이터를 기반으로 각종 계측 값을 산출한 뒤, **tableData와 oneSecList 형태로 가공하여 외부로 전달한다.**

- `oneSecMainWindow`는 `oneSecCalcData` 객체를 내부에서 생성하고 관리하며, 그 생명주기를 책임지는 주체로 동작한다. 즉, `oneSecCalcData`는 `oneSecMainWindow`에 종속되어 있으며, 창이 생성될 때 함께 생성되고, 소멸할 때 함께 정리되는 **강한 포함(composition) 관계**를 가진다.

#### 5. A3700N_GUI

<p align="center">
      <img src="https://github.com/hks3274/power-Simulation/blob/main/diagram_images/power_simulation_%ED%81%B4%EB%9E%98%EC%8A%A4_a3700.PNG?raw=true" width="50%">
</p>

> - `A3700N_GUI` 클래스는 시뮬레이션 결과를 사용자에게 최종적으로 제공하는 마지막 단계의 출력 클래스이다. 이 클래스는 1초 단위 데이터 창에서 전달된 값을 기반으로, 더욱 확장된 형태의 전력 계측 정보를 시각화하여 보여준다. 

- 앞서 설명한 `oneSecCalcData`에서 전달되는 `oneSecList` 데이터를 받아 이를 다양한 출력 위젯에 반영함으로써 사용자에게 종합적인 시뮬레이션 결과를 제공한다.

- 다만 파형, 고조파, Phasor와 같은 시각적 요소는 단순히 oneSecList 데이터만 받아 표시하는 방식으로 구현할 수 없기 때문에, 각각을 담당하는 별도의 위젯을 구성해야 한다. 전압·전류의 평균값, 유효전력, 무효전력, 역률과 같은 수치 기반 전력 분석 정보는 oneSecCalcData에서 계산된 값을 그대로 가져와 표시할 수 있지만,  
**파형(waveform)** 은 **QCustomPlot**을   
**위상도(Phasor)** 는 **QPainter** 기반의 사용자 정의 그리기 로직을   
**고조파(harmonic)** 는 막대 그래프 형태를 사용해야 하므로 **QCustomPlot의 bar graph** 기능을   
각각 활용해야 한다. 따라서 이러한 다양한 표시 방식과 그래픽 도구를 처리하기 위해, `A3700N_wave`, `A3700N_phasor`, `A3700N_harm`과 같이 전용 위젯 클래스를 별도로 설계하는 것이 필수적이다.

- `A3700N_wave`는 앞서 설명한 것처럼 **QCustomPlot**을 사용해 시뮬레이션된 파형(Waveform)을 시각화하는 위젯이다. 이 출력 창에서는 실시간 파형 전체를 모두 표시하는 대신, 1초가 종료된 시점에서의 최신 2주기만을 추출하여 보여주도록 설계하였다. 이를 통해 사용자는 해당 시점의 파형 형태를 보다 명확하게 파악할 수 있다. 또한 `dataChanged()` 시그널을 수신할 때마다 내부적으로 한 주기를 계산하고, 그 결과를 기반으로 파형을 갱신한다.

- `A3700N_phasor`는 **QPainter**를 활용해 각 상(Phase)의 위상 관계와 크기를 시각적으로 표현하는 위젯이다. 이 출력은 1초가 지날 때마다 최신 Phasor 값을 가져와 갱신하는 방식으로 동작하며, 내부적으로 `rmsDataChanged()` 시그널을 통해 업데이트가 이루어진다. 다이어그램에는 표기되어 있지 않지만, 실제 구현에서는 전용 계산 클래스인 `phasorCalculator`에서 Phasor 데이터를 산출하고, A3700N_phasor는 이를 받아 위상도를 그려낸다.

- `A3700N_harm`은 **QCustomPlot**의 **Bar Graph** 기능을 활용하여 각 차수별 고조파(harmonic) 성분의 크기를 시각적으로 표현하는 위젯이다. 이 출력 창에서는 1초가 경과했을 때의 최신 고조파 데이터를 가져와 막대 그래프로 표시하며, 이를 통해 사용자가 전체 고조파 분포를 한눈에 확인할 수 있도록 한다. 또한 `rmsDataChanged()` 시그널을 수신할 때마다 새로운 데이터를 반영해 그래프를 갱신하는 방식으로 동작한다.

#### 6. 기타 구조
- `inputMainWindow`, `MainWindow`, `oneSecMainWindow`, `A3700N_GUI`는 서로 **약한 포함(aggregation) 관계**를 가진다. 각 창은 독립적으로 생성·소멸할 수 있으며, 특정 윈도우가 닫히더라도 다른 윈도우의 생명주기에는 영향을 주지 않는다. 이를 위해 전체 메뉴바에 **창 열기(Open Window)** 기능을 구현하여 사용자가 원하는 시점에 개별 윈도우를 활성화할 수 있도록 하였고, 각 윈도우 내부에서도 다른 창을 열고 닫을 수 있는 메뉴를 제공함으로써 유연한 UI 구성과 독립적인 창 관리가 가능하도록 설계하였다.

#### 7. 자세한 구조
<p align="center">
      <img src="https://github.com/hks3274/power-Simulation/blob/main/diagram_images/power_simulation_%ED%81%B4%EB%9E%98%EC%8A%A4%EB%8B%A4%EC%9D%B4%EC%96%B4%EA%B7%B8%EB%9E%A8_%EC%9E%90%EC%84%B8%ED%9E%88.PNG?raw=true" width="80%">
</p>

- 다음은 전체 시스템의 구조를 보다 상세하게 표현한 클래스 다이어그램이다. struct 형태로 이루어진 방대한 데이터 타입들은 복잡도를 줄이기 위해 다이어그램에 모두 포함하지는 않았지만, 각 클래스 간 관계와 책임 범위를 파악하기에는 충분한 수준의 정보를 제공한다. 다이어그램에서 보이는 생소한 타입 대부분은 프로젝트 내에서 정의한 사용자 지정 구조체이며, 자세한 내용은 **GitHub의 dataStructures.h 파일을 참고**하면 된다.


---

### 3. 데이터 & 구조의 흐름 
<p align="center">
      <img src="https://github.com/hks3274/power-Simulation/blob/main/diagram_images/power_simulation_%EC%8B%9C%ED%80%80%EC%8A%A4.png?raw=true" width="80%">
</p>

>- 이 다이어그램은 전체 데이터 처리 과정과 클래스 간 상호작용을 시간 흐름에 따라 나타낸 **시퀀스 다이어그램**이다. 시뮬레이션이 어떤 순서로 진행되는지를 직관적으로 이해할 수 있다. 이제 이 다이어그램을 기반으로 시스템의 전체 동작 흐름을 살펴보도록 한다.

#### 1. Uset --> input --> dataManagement
<p align="center">
      <img src="https://github.com/hks3274/power-Simulation/blob/main/diagram_images/power_simulation_%EC%8B%9C%ED%80%80%EC%8A%A4_input.PNG?raw=true" width="70%">
</p>
 
- 사용자는 `inputMainWindow`에 배치된 위젯들을 통해 전압, 전류, 고조파 등의 **입력 값을 변경**한다.

- 각 위젯은 변경된 값을 즉시 `dataManagement`의 **`setter` 함수로 전달**하여 시뮬레이션 파라미터를 갱신한다.

- `dataManagement`는 전달받은 값을 **내부 변수에 반영**하고, 이후의 샘플 계산과 주기 계산에 이 새로운 값이 적용되도록 한다.


#### 2. QChronoTimer --> datamanagement
    
<p align="center">
      <img src="https://github.com/hks3274/power-Simulation/blob/main/diagram_images/power_simulation_%EC%8B%9C%ED%80%80%EC%8A%A4_timer.PNG?raw=true" width="30%">
</p>

- 실시간 동작에서는 `QChronoTimer`가 설정된 **interval_time마다 timeout()을 발생**시키며, 이 이벤트를 기반으로 `dataManagement`에서는 `dataChanged()` 또는 `rmsDataChanged()` **시그널을 발생**시킨다. 이렇게 전달된 시그널은 각기 연결된 위젯과 클래스에 전달되어 파형 갱신, RMS 계산, Phasor 업데이트 등 다양한 **후속 처리를 수행**하게 된다.


#### 3. datamanagement --> signal --> output 

<p align="center">
      <img src="https://github.com/hks3274/power-Simulation/blob/main/diagram_images/power_simulation_%EC%8B%9C%ED%80%80%EC%8A%A4_output.PNG?raw=true" width="70%">
</p>

- `dataChanged()` 시그널이 발생하면 `graphWidget`에서 이를 수신하여 `update()`를 실행하고, 가장 최신 샘플 기반의 파형을 다시 그린다.

- `rmsDataChanged()` 시그널은 `RMSGraphWidget`과 `phasorWidget` 모두로 전달되며, 각각 **RMS 그래프와 Phasor 시각화를 갱신**한다.

- 특히 `phasorWidget`은 해당 시그널을 수신하면 내부적으로 `calcPhasor()`를 호출해 최신 위상 값을 계산하고, 이를 반영하여 화면을 `update()` 한다.

- `MainWindow`에서 업데이트된 위젯을 실시간으로 보여준다.

#### 4. oneSecCalcData --> oneSecMainWindow 

<p align="center">
      <img src="https://github.com/hks3274/power-Simulation/blob/main/diagram_images/power_simulation_%EC%8B%9C%ED%80%80%EC%8A%A4_oneSec.PNG?raw=true" width="30%">
</p>

-  위의 과정과 동시에 `oneSecCalcData` 역시 같은 한 주기 시그널(`rmsDataChanged()`)을 받아 1초 누적 데이터를 갱신하고, 필요 시 tableData 및 oneSecList를 생성해 후속 처리 단계를 진행한다.

- 이때 `oneSecCalcData`에서 생성된 `tableData`는 `oneSecMainWindow`에서 테이블 UI를 구성하는 데 사용되며, 사용자가 1초 단위의 계측 정보를 확인할 수 있도록 표시된다. 반면 `oneSecList`는 이후 단계에서 설명할 `A3700N_GUI` 및 관련 위젯들에 전달되어 출력 화면을 구성하는 데 활용된다.

#### 5. oneSecMainWindow --> A3700N_GUI

<p align="center">
      <img src="https://github.com/hks3274/power-Simulation/blob/main/diagram_images/power_simulation_%EC%8B%9C%ED%80%80%EC%8A%A4_a3700n_%EB%B3%80%EA%B2%BD.PNG?raw=true" width="60%">
</p>

- `A3700N_GUI`는 `oneSecCalcData`로부터 전달된 `oneSecList` 데이터를 기반으로 화면에 결과를 출력하는 역할을 수행한다. 다만 파형(Wave), Phasor, 고조파(Harmonics)와 같이 그래픽 표현이 필요한 요소들은 각각 다른 방식(QCustomPlot, QPainter, Bar Graph 등)을 사용해야 하므로, 이를 위해 별도의 전용 위젯을 구성하여 데이터를 갱신하도록 설계하였다.

- 이때 `A3700N_wave`는 `dataChanged()` 시그널을 받아 최신 파형을 업데이트하며, `A3700N_phasor`와 `A3700N_harm`은 `rmsDataChanged()` 시그널을 수신하여 Phasor와 고조파 그래프를 각각 갱신한다.

---

### 4. 데이터 계산

- 본 프로젝트에서 사용된 주요 계산식들은 다음과 같다. 각 수식은 전압·전류의 기본파 분석, RMS 계산, 고조파 분해, 전력 분석(유효전력, 무효전력, 역률) 등의 항목을 산출하기 위해 적용되었으며, 시뮬레이션의 핵심 로직을 구성한다.

#### 1. 기본 사인파 출력 
- 전압·전류의 시간 영역 파형은 다음의 사인 함수로 표현된다.

```javascript
v(t) = A ⋅ sin( 2πft + φ )
```
    - A : 진폭(Amplitude) — 파형의 최대값
    - f : 주파수(Hz)
    - ϕ : 위상(Phase)
    - t : 시간(Time)

#### 2. rms 계산
- 한 주기 동안의 전압·전류 크기를 나타내는 지표로, 다음과 같이 계산된다.
 ```javascript
    
```


#### 3. phasor 계산 (DFT)

#### 4. 고조파 계산 (FFT)

#### 5. 각종 계측 항목 계산




