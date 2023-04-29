function decoded_bits_packet_rx = func_conv_decoding(received_bits_packet_rx)
    
    decoded_bits_packet_rx=zeros(length(received_bits_packet_rx)/2,1);
    
    V0=0; V1=0; V2=0; V3=0;    
    for ind = 1:length(decoded_bits_packet_rx)
        
        codeword=received_bits_packet_rx(2*ind-1:2*ind);
        metric_00=sum(double(codeword~=[0;0]));
        metric_01=sum(double(codeword~=[1;1]));
        metric_02=sum(double(codeword~=[0;1]));
        metric_03=sum(double(codeword~=[1;0]));
        metric_10=sum(double(codeword~=[1;1]));
        metric_11=sum(double(codeword~=[0;0]));
        metric_12=sum(double(codeword~=[1;0]));
        metric_13=sum(double(codeword~=[0;1]));
        if ind==1
            V0=metric_00;
            M0=[0];
            V2=metric_10;
            M2=[1];
        else
            if ind==2
                V0_t=V0+metric_00;
                M0_t=[M0;0];
                V1_t=V2+metric_02;
                M1_t=[M2;0];
                V2_t=V0+metric_10;
                M2_t=[M0;1];
                V3_t=V2+metric_12;
                M3_t=[M2;1];
            else
                % Compute path metric for each state
                %=======================================================================
                V0_t = min(V1+metric_10,V0+metric_00);
                if V0_t == V1+metric_10
                    i0 = 1;
                else
                    i0 = 2;
                end
                
                V1_t = min(V2+metric_02,V3+metric_12);
                if V1_t == V2+metric_02
                    i1 = 1;
                else
                    i1 = 2;
                end
               
                V2_t = min(V1+metric_00,V0+metric_10);
                if V2_t == V0+metric_10
                    i2 = 1;
                else
                    i2 = 2;
                end
                
                V3_t = min(V2+metric_03,V3+metric_13);
                if V3_t == V2+metric_03
                    i3 = 1;
                else
                    i3 = 2;
                end
                % Write code here
                %=======================================================================
                M0_t=[M1*(i0==1)+M0*(i0==2);0];
                M1_t=[M2*(i1==1)+M3*(i1==2);0];
                M2_t=[M0*(i2==1)+M1*(i2==2);1];
                M3_t=[M2*(i3==1)+M3*(i3==2);1];
            end
            V0=V0_t; M0=M0_t;
            V1=V1_t; M1=M1_t;
            V2=V2_t; M2=M2_t;
            V3=V3_t; M3=M3_t;
            
        end
    end
% Find the decoder output by comparing the final path metrics
%=======================================================================
% Write code here
%=======================================================================    
end