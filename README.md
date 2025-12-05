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

>- **입력 UI를 담당**하는 핵심 클래스들이며, 사용자가 전압, 전류, 고조파 등 다양한 파라미터를 직접 설정하는 역할을 수행한다. 이곳에서 사용자가 변경한 모든 값은 즉시 `dataManagement`로 전달되어 전체 시뮬레이션 과정에 반영된다. 또한 각 입력 위젯은 `QDockWidget` 형태로 `inputMainWindow`에 존재해, 사용자가 원하는 위치로 자유롭게 배치하거나 분리할 수 있도록 설계하였다. 

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

>- **출력 화면을 담당**하는 핵심 클래스들로, 사용자가 실제로 확인하는 그래프·수치·파형 등의 UI 요소를 구성한다. 이 클래스들은 `dataManagement`로부터 전달된 데이터를 받아 필요한 형태로 가공한 뒤 화면에 표시하는 역할을 수행한다. 또한 각 출력 위젯은 `MainWindow` 내부에서 `QDockWidget` 형태로 배치되므로, 사용자가 창의 위치를 자유롭게 이동하거나 분리하여 사용할 수 있다.

- `graphWidget`은 시뮬레이션된 3상 전압, 전류 합성파를 매 샘플 주기마다 실시간으로 그래프로 표시하는 역할을 수행한다. `dataManagement`에서 발생하는 `dataChanged()` 시그널이 바로 한 샘플 주기마다 발생하는 신호이기 때문에, graphWidget은 이 시그널을 수신할 때마다 최신 파형 데이터를 받아와 **QCustomPlot**을 통해 화면에 즉시 갱신한다. 이를 통해 사용자는 합성된 전압·전류 파형을 확인할 수 있다.

- `RMSGraphWidget`은 파형의 한 주기가 완료될 때마다 RMS 값을 계산하여 그래프로 시각화하는 역할을 담당한다. `dataManagement`는 한 주기가 끝날 때마다 `rmsDataChanged()` 시그널을 발생시키며, `RMSGraphWidget`은 이 시그널을 수신하여 최신 RMS 데이터를 받아온다. 이를 바탕으로 3상 전압·전류의 RMS, 고조파 RMS, 그리고 고조파가 포함된 3상 합성 RMS 파형까지 함께 그래프로 표시한다. 이 그래프 또한 **QCustomPlot**을 사용해 화면에 갱신한다. 

- `phasorWidget` 역시 한 주기가 완료될 때마다 갱신되며, 각 상의 위상 및 크기를 Phasor 형태로 시각화하는 역할을 수행한다. 실제 계산된 Phasor 값은 별도의 `phasorCalculator` 클래스에서 제공되지만(요약 구조에서는 생략), `phasorWidget`은 `rmsDataChanged()` 시그널을 통해 주기별 최신 값을 받아와 화면에 표시하도록 설계되어 있다.

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

