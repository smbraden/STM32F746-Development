% Purpose:      converts a floating point number to binary with the given 
%               number of integer and floating point binary digits

% Example... 
% 
% Suppose I am seeking the Mantissa and Fractional components of the divisor 
% needed to configure a UART peripheral to the desired Baud rate of 9600
% Each component (int and float) goes to a specific register position

% bin = [0,1,1,0,1,0,0,0,0,0,1,0,1,0,1,0]
% intBin = [0,1,1,0,1,0,0,0,0,0,1,0]
% floatBin = [1,0,1,0]

floatNum = (16*10^6)/9600;  % your float point number
intDigits = 12;             % number bits for integer part of your number      
floatDigits = 4;             % number bits for fraction part of your number


% binary number
bin = fix(rem(floatNum * pow2(-(intDigits - 1): floatDigits),2)); 
% the inverse transformation
dec = bin * pow2(intDigits - 1:-1:-floatDigits).';

intBin = bin(1 : intDigits);
floatBin = bin((intDigits + 1) : end);