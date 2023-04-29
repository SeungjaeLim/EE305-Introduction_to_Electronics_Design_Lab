function symbols_packet_tx = func_QPSK_modulation(encoded_bits_packet_tx)

    j=sqrt(-1);
    number_of_bits_per_symbol = 2;
    len= length(encoded_bits_packet_tx)/number_of_bits_per_symbol;
    
    symbols_packet_tx = zeros(len,1);
    
    b00 = [0; 0];
    b01 = [0; 1];
    b10 = [1; 0];
    b11 = [1; 1];
    
    s00 = (1+j)/sqrt(2);
    s01 = (1-j)/sqrt(2);
    s10 = (-1+j)/sqrt(2);
    s11 = (-1-j)/sqrt(2);    
    
    for ind=1:len
    %=======================================================================  
    % Perform QPSK modulation (cf. PCM)
    n = number_of_bits_per_symbol;
    k = encoded_bits_packet_tx(n*ind-1:n*ind,1);
    if k == b00
        symbols_packet_tx(ind,1) = s00;
    elseif k ==b01
        symbols_packet_tx(ind,1) = s01;
    elseif k == b10
        symbols_packet_tx(ind,1) = s10;
    elseif k == b11
        symbols_packet_tx(ind,1) = s11;
    end
    %=======================================================================    
    end

end