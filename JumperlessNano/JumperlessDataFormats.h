
This is just to make a mental map or how an arduino board
plugged into jumperless will communicate what connections
to make to the onboard AVR32DD32 controller via I2C or 
UART. We'll use the Stream library so the calls will be
the same either way. There should be a sections for each 
of these things:

Data format - different message types/headers to be sent over I2C or UART to the control chip

Let's start with UART


  #Dumb mode#                                                     *maybe start with this because it's way easier*
    Dumb mode where pathfinding is done on the Nano and connections are sent raw 
      Messages will just be: crosspoint chip (A-K), Y connection (0-15), X connection(0-7), connect or disconnect (1/0), reset (y/n), DAC output (0-1023)
        Storing connections and stuff will be the responsibility of the Nano and 
        DAC could just be included in the same message above for simplicity


@jumperless -dumb <A,12,3,1> <B,2,3,1> @end
sentinel     mode ch,X ,Y,c 

  #Smart mode#
    Connections/disconnections

      Connect/disconnect from Nano to breadboard (I,J,K chips) this may call the BB connection function below
        Return number of hops, estimated resistance

      Connect/disconnect within the breadboard (A-H chips) optional priority and redundancy value (make parallel connections)
        Return number of hops, estimated resistance

      Load an entire state
        Send @A0, (connection), (connection), @A1, (connection), etc.  @row0, (connection), @row1, @row2, (connection), (connection), etc.

      Reset all connections

      Find a path and return it without actually making any connections



    DAC setting/waveform generation (maybe store some lookup tables for various waves on the controller)

      Send custom waveform lookup table (or equation to make it on the fly - that's distant future shit though)

      Request the voltage setting on DAC



    Request state of the matrix (Backend or frontend, how many open connections there are left and maybe some sort of "utilization score")

      Low Level request per chip (so send A-K) 

          returns X[16] {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, Y[8] {0,0,0,0,0,0,0,0}  //0 = free  1 = used for that chip


      High level style request for the board

        returns breadboardRows{0-60} @row0,(if no connections send nothing after it), @row1, (send comma seprated rows it's connected to, and nano header labels), @row3, etc. $end
                This is a tricky one, because there shouldn't reall be a maximum number of connections per row if we use all the tricks to get more connections
                I guess there's some theoretical maximum that I should figure out (probably like ~40ish idk), but we don't want to allocate memory locations for the theoretical maximum 
                number of connections 60 times. It feels wasteful considering the reasonable maximum will be like ~5. 
                I recently redid this as a linked list where each connection is a new entry so it's only storing connections. So maybe that's fine.
            
                I think these should be human readable, so maybe send something like @row1,(connections) so the values following aren't confused with moving onto the next row
                and we can do this for the nano connections as text too, so @A0, (connection), (connection), @A1, (connection), @A2, @A3, etc.
                So the @prefix will say "we're listing connnections for the following row/nano connection" and things following without the @ will be interpreted as connections


      High level style request for the nano (same data as above but from the nano's POV instead)

          returns nanoHeaderPins{0-30} @A0,(if no connections send nothing after it), @A1, (send comma seprated rows it's connected to, and other nano header labels), @A3, etc. $end
                We should probably keep the linked list containing the connections in order so we don't have to search the whole thing every time
                or maybe initialize it with an empty connection for every row so things can be easily found and placed into the list?


      High level style request for a single row/nano pin

        send @row/nano pin
          returns @row/nano pin, (connection (without the @ prefix)), (connection), etc. then some sort of EOF signal, maybe $end or something like that


      Request for which UNCONNECTED_(A-H) are free (send whether you want yes/no or what they're connected to) this will be used a lot in hop finding

        if you just want free/used
          returns UNCONNECTED[8] {0,0,0,0,0,0,0,0}  // 0 for free, 1 for used

        if you want what they're connected to it will be similar to above
          returns @UNCONNECTED_A, (connection), (connection), (connection), @UNCONNECTED_B, (connection), @UNCONNECTED_C, etc. , $end



    Change jumper settings

      State of the solder jumpers that decide whether the unconnected BB rows are on both sides or the far right

      Whether to disable the op amp (cause the power supply jumpers are cut) or not and and not worry about it

      Voltage setting jumpers on the top and bottom rails (+3.3V, 5V, DAC+-9V, disconnected/external)

      Maybe UPDI programming jumper but I'm not sure that even matters



    Change I2C address, speed or UART baud setting



Storage of the state (on the controller chip or Nano? probably the controller with an option for the nano)

  TODO: this isn't fully thought out but I need to go fix a water heater right now

  I think we should keep a sorted linked list with an entry for each new connection like I've already done

  Here's the struct from the current Jumperless code (it may need to be reworked to account for the nano headers, or have a separate one for them)
  
      struct connection { //I can't decide whether to store both hops in the same struct but for now I will

              bool used;    //I guess if there's an entry for it, used will always be true

              int row1;     //this needs to be changed to account for the nano pins now
              int row2;

              int chip1;    
              int chip2;

              int chip1Xaddr;
              int chip1Yaddr;

              int chip2Xaddr;
              int chip2Yaddr;

              int lane;     //each chip has 2 connections to every other chip (almost, I took out one of the lanes for chips that are directly across
                            //from each other to make room for the K special function chip) values are 0 or 1 so it could be a bool

              int hop;      //also could be a bool unless we allow double hops in which case we need more hopChip entries below

              int hopChip;
              int hopChipXaddr1; //we know the Y address will be 0 because UNCONNECTED_(A-H) are all at Yaddr 0, but maybe it should be added for clarity
              int hopChipXaddr2;


    };