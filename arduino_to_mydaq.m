dq2 = daq("ni");
dq2.Rate = 1000000;
addoutput(dq2, "myDAQ1", "ao0", "Voltage");
% setting up mydaq for input from the arduino
input_line = daq("ni");
input_line.Rate = 8000;
addinput(input_line, "myDAQ1", "port0/line0", "Digital");
while(true)
   a = read(input_line);
   output = a{1,"myDAQ1_port0/line0"};
   if(output ==1)
       % contraction code
       % big contraction for the bottom of the stem of the B
       % parameters for the sine wave signal
       frequency = 2000; % contraction frequency
       amplitude = 1; % keep a lower amplitude for contraction
       duration = 1;
       samples = dq2.Rate * duration; % total number of samples
       t = linspace(0, duration, samples); % time vector
       % generate the full sine wave signal using the time vector
       outputSignal = amplitude * sin(2 * pi * frequency * t);
       start(dq2);
       write(dq2, outputSignal'); % transpose to ensure correct shape (column vector)
       pause(duration);
       stop(dq2);
       output = 0;
   end
end


