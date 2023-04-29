function bit_stream_tx = func_PCM_coding(data, bitsPerSample)

    bit_stream_tx = zeros(length(data)*bitsPerSample,1);
    bits = zeros(bitsPerSample,1);
    
    for ind=1:length(data)
        value = data(ind);  % data value ranges from -1 to 1
        value = (2^(bitsPerSample-1)-0.5) * value; % scaling
        
        %=======================================================================
        %PCM coding
        value = floor(value);
        if value<0
            value = value + 2^(bitsPerSample);
        end
        for i = 1:bitsPerSample
            bits(bitsPerSample-i+1) = rem(value,2);
            value = floor(value/2);
        end
        %=======================================================================
        
        bit_stream_tx( (ind-1)* bitsPerSample +1: ind* bitsPerSample ) = bits;
       
    end
end