%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%       Matlab Script                 %
%       EE305, Lab 7 & 8              %
%       Spring 2022                   %
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% The file 'voice.mat' includes a 20-second long piece of speech sampled at
% 8kHz under the matlab array named 'data' with values contained in [-1,1].

% Our goal in labs 7 & 8 are to perform
%  1. Perform PCM waveform encoding from matlab array 'data'
%  2. Perform QPSK modulation and demodulation
%  3. Perform convoluational encoding and decoding
%  4. Assess voice quality for different SNR values

clear all;
clc;

j = sqrt(-1);
f_sample = 8000;  % samples per second
bitsPerSample = 3; % PCM coding parameter
block_size = 128;  % number of bits per packet
EbNo_array_dB = [0:2:10];



%%%%%%%%%%%%% Load the data
load voice;
plot(data); % check data
disp('data size')
size(data)
data_array_rx=zeros(size(data));


%%%%%%%%%%%%% PCM Waveform encoding
%disp('Under PCM coding')
bit_stream_tx = func_PCM_coding(data, bitsPerSample);
%%%% Insert script:  Use 8bit O2C offset 2's complement binary scheme
%%%% to represent a value x[n], where -1 <= x[n] <= 1
%%%% data => s[n]
disp('bit stream tx size')
size(bit_stream_tx)
number_of_packets = length(bit_stream_tx)/block_size;
bit_stream_rx = zeros(length(bit_stream_tx),1);


BER = zeros(length(EbNo_array_dB),1);
BER_theo = zeros(length(EbNo_array_dB),1);

for iebno=1:length(EbNo_array_dB)

    EbNo_dB=EbNo_array_dB(iebno);
    EbNo_dB
    EbNo = 10^(0.1*EbNo_dB);
    %calcuate signal gain and noise variance from EbNo:  Eb/No = 0.5 Es/No = 0.5 * gain^2/No;
    % sigma^2 = No/2
    %=======================================================================
    gain = 1;
    No = ((0.5)*(gain)^2)/EbNo;
    sigma = sqrt(No/2);
    %=======================================================================
%     sigma=0.1;

    for ipacket=1:number_of_packets

        %%%%%%%%%%%%% Take a packet from bit_stream_tx
        bits_packet_tx = bit_stream_tx( (ipacket-1)*block_size+1: ipacket*block_size );

        %%%%%%%%%%%%% Convolutional Channel Coding
        encoded_bits_packet_tx = func_conv_coding(bits_packet_tx);
        %encoded_bits_packet_tx =  bits_packet_tx;

        %%%%%%%%%%%%% QPSK modulation
        %generate symbol_block of complex symbols with unit magnitude
        symbols_packet_tx = func_QPSK_modulation(encoded_bits_packet_tx);

        %%%%%%%%%%%%% AWGN and Receiver
        %=======================================================================
        % AWGN
        syn_block_size = length(symbols_packet_tx);
        awgn = (randn(sym_block_size, 1) + j * randn(sym_block_size, 1));
        symbols_packet_tx = symbols_packet_tx+ sigma * awgn;
        %=======================================================================
         symbols_packet_rx =  symbols_packet_tx;

        %%%%%%%%%%%%% Hard-decision QPSK demodulation
        received_bits_packet_rx = func_QPSK_demodulation(symbols_packet_rx);

        %%%%%%%%%%%%% Convolutional Decoding
        decoded_bits_packet_rx = func_conv_decoding(received_bits_packet_rx);
        %decoded_bits_packet_rx = received_bits_packet_rx;

        %%%%%%%%%%%%% Reconstruct the whole file
        bit_stream_rx((ipacket-1)*block_size+1: ipacket*block_size) = decoded_bits_packet_rx;

    end

    %=======================================================================
    % For each Eb/No, compute empirical and theoretical BER

    %=======================================================================
    numBE = sum(ne(bit_stream_tx,bit_stream_rx));
    BER(iebno) = numBE/length(bit_stream_tx);
    BER_theo(iebno) = 0.5*erfc(sqrt(EbNo));
    % PCM decoding and replay the sound
    disp('bit stream rx size')
    size(bit_stream_rx)
    data_array_rx = func_PCM_decoding(bit_stream_rx,bitsPerSample);
    plot(data_array_rx);
    sound(data_array_rx,8000);% generate sound using the array 'data' with 8kHz sampling rate with 8 bit resolution.

    disp('paused after one iebno iteration')
    pause

end

%% plot BER
figure (1)
semilogy(EbNo_array_dB,BER,'--x')
hold on
grid on
semilogy(EbNo_array_dB,BER_theo,'-o')
xlabel('Eb/No [dB]');
ylabel('BER');
