function encoded_bits_packet_tx = func_conv_coding(bits_packet_tx)

    encoded_bits_packet_tx=zeros(length(bits_packet_tx)*2,1);
    
    % initial state
    state_0=0;
    state_1=0;
    
    A=[1 0 1];
    B=[1 1 1];
    
    for ind=1:length(bits_packet_tx)

        % encoded bits
        %=======================================================================
        input_bit = bits_packet_tx(ind);
        encoded_bits_packet_tx(2*ind-1) = xor(input_bit,state_1);
        encoded_bits_packet_tx(2*ind) = xor(input_bit,xor(state_0,state_1));
        % Write code here
        %=======================================================================
       
        % memory update
        %=======================================================================
        state_1 = state_0;
        state_0 = input_bit;
        % Write code here
        %=======================================================================

    end
end
