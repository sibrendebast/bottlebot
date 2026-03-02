::: mermaid
stateDiagram-v2
    %% Define styles
    classDef safe fill:#27ae60,color:white,font-weight:bold,stroke:#2ecc71,stroke-width:2px;
    classDef action fill:#2980b9,color:white,stroke:#3498db,stroke-width:2px;
    classDef danger fill:#c0392b,color:white,font-weight:bold,stroke:#e74c3c,stroke-width:2px;

    %% Nodes
    [*] --> STATE_0_IDLE
    
    STATE_0_IDLE --> STATE_1_INDEXING : Receive <START> command \n (Conveyor ON, Exit Gate Blocked, Entry Gate Open)
    
    STATE_1_INDEXING --> STATE_2_LOWER_HEADS : 4 Bottles Detected \n (Entry Gate Closes)
    STATE_1_INDEXING --> ERROR_HALT : Timeout (Missing Bottle)
    
    STATE_2_LOWER_HEADS --> STATE_3_CO2_FLUSH : 'Heads Down' Switch Triggered
    STATE_2_LOWER_HEADS --> ERROR_HALT : Timeout (Piston Jam)
    
    STATE_3_CO2_FLUSH --> STATE_4_BEER_FILL : CO2 Timer Elapsed \n (Rod Power ON)
    
    STATE_4_BEER_FILL --> STATE_5_RAISE_HEADS : All Probes Triggered \n (Rod Power OFF, Angle Seats Close)
    
    STATE_5_RAISE_HEADS --> BOTTLE_CLEAR : 'Heads Up' Switch Triggered \n (Exit Gate Opens)
    STATE_5_RAISE_HEADS --> ERROR_HALT : Timeout (Piston Jam)

    BOTTLE_CLEAR --> STATE_1_INDEXING : Area Clear & No [STOP] \n (Exit Gate Closes, Entry Gate Opens)
    BOTTLE_CLEAR --> STATE_0_IDLE : Area Clear & [STOP] Received \n (Exit Gate Closes, Conveyor OFF)
    
    ERROR_HALT --> STATE_0_IDLE : Operator Clears Error & Sends <START>

    %% Apply Styles
    class STATE_0_IDLE safe
    class STATE_1_INDEXING,STATE_2_LOWER_HEADS,STATE_3_CO2_FLUSH,STATE_4_BEER_FILL,STATE_5_RAISE_HEADS action
    class ERROR_HALT danger

    %% Note for Global E-Stop
    note right of STATE_0_IDLE
        GLOBAL INTERRUPT:
        If <ESTOP> is received at ANY time,
        or UART connection is lost,
        immediately close all valves
        and jump to ERROR_HALT.
    end note
:::