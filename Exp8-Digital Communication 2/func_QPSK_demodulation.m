function received_bits_packet_rx = func_QPSK_demodulate(symbols_packet_rx)

    j=sqrt(-1);
    number_of_bits_per_symbol = 2;
    len= length(symbols_packet_rx);
    
    received_bits_packet_rx= zeros(len*number_of_bits_per_symbol,1);
    
    b00 = [0; 0];
    b01 = [0; 1];
    b10 = [1; 0];
    b11 = [1; 1];
    
    s00 = (1+j)/sqrt(2);
    s01 = (1-j)/sqrt(2);
    s10 = (-1+j)/sqrt(2);
    s11 = (-1-j)/sqrt(2);    
    
    for ind=1:len
        
        symbol_rx = symbols_packet_rx(ind);
        Rs = real(symbol_rx);
        Is = imag(symbol_rx);
        
        %=======================================================================
        % Perform QPSK demodulation (cf. PCM decoding)
        if Rs >=0 && Is>=0
            received_bits_packet_rx(number_of_bits_per_symbol*ind-1:number_of_bits_per_symbol*ind,1) =b00;
        elseif Rs>= 0 && Is <0
            received_bits_packet_rx(number_of_bits_per_symbol*ind-1:number_of_bits_per_symbol*ind,1) =b01;
        elseif Rs<=0 && Is>=0
            received_bits_packet_rx(number_of_bits_per_symbol*ind-1:number_of_bits_per_symbol*ind,1) =b10;
        elseif Rs<0 && Is<0 
            received_bits_packet_rx(number_of_bits_per_symbol*ind-1:number_of_bits_per_symbol*ind,1) =b11;
        end
        %=======================================================================
        
    end


end