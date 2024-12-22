% This script is used to read the binary file produced by the DCA1000
% and Mmwave Studio
% Command to run in Matlab GUI -readDCA1000('<ADC capture bin file>')
% -------------------------------------------------------------------------------------------------------
function [retVal] = read1843Data(fileName, numADCSamples, numRX)

% global variables
% change based on sensor config
numADCBits = 16; % number of ADC bits per sample
numLanes = 2; % do not change. number of lanes is always 2
% -------------------------------------------------------------------------------------------------------

% read .bin file
fid = fopen(fileName,'r');
adcData = fread(fid, 'int16');
fclose(fid);
% -------------------------------------------------------------------------------------------------------

fileSize = size(adcData, 1);
LVDS = zeros(1, fileSize/2);
%combine real and imaginary part into complex data
%read in file: 2I is followed by 2Q
ptr = 1;
% I1 I2 Q1 Q2 I3 I4 Q3 Q4

for i=1:4:fileSize-1
    LVDS(1,ptr)   = adcData(i)   + 1j*adcData(i+2); 
    LVDS(1,ptr+1) = adcData(i+1) + 1j*adcData(i+3); 
    ptr = ptr + 2;
end

% 每隔400帧进行8帧的切割
% 每隔256*3*400的样本点，切割掉256*3*8的样本点

% R1 R2 R3 R4
% R1 R2 R3 R4 R1 R2 R3 R4 
% filesize = 2 * numADCSamples*numChirps
numChirps = fileSize/2/numADCSamples/numRX;
% create column for each chirp
LVDS = reshape(LVDS, numADCSamples*numRX, numChirps);
% each row is data from one chirp
LVDS = LVDS.';
% -------------------------------------------------------------------------------------------------------

%organize data per RX
retVal = zeros(numRX,numChirps*numADCSamples);
for r_idx = 1:numRX
    for c_idx = 1:numChirps
        retVal(r_idx,(c_idx-1)*numADCSamples+1:c_idx*numADCSamples) = ...
        LVDS(c_idx,(r_idx-1)*numADCSamples+1:r_idx*numADCSamples);
    end
end

% 
% function [retVal] = read1843Data(fileName, numADCSamples, numRX)
% 
% % global variables
% % change based on sensor config
% numADCBits = 16; % number of ADC bits per sample
% numLanes = 2; % do not change. number of lanes is always 2
% % -------------------------------------------------------------------------------------------------------
% 
% % read .bin file
% fid = fopen(fileName,'r');
% adcData = fread(fid, 'int16');
% fclose(fid);
% % -------------------------------------------------------------------------------------------------------
% 
% fileSize = size(adcData, 1);
% LVDS = zeros(1, fileSize/2);
% 
% % Combine real and imaginary parts into complex data
% % Read in file: 2I is followed by 2Q
% ptr = 1;
% % I1 I2 Q1 Q2 I3 I4 Q3 Q4
% 
% for i=1:4:fileSize-1
%     LVDS(1,ptr)   = adcData(i)   + 1j*adcData(i+2); 
%     LVDS(1,ptr+1) = adcData(i+1) + 1j*adcData(i+3); 
%     ptr = ptr + 2;
% end
% 
% % 每隔400帧进行8帧的切割
% % 每隔256*3*400的样本点，切割掉256*3*8的样本点
% 
% numChirps = fileSize / 2 / numADCSamples / numRX;  % Calculate number of chirps
% % Create column for each chirp
% LVDS = reshape(LVDS, numADCSamples * numRX, numChirps);
% LVDS = LVDS.';  % Transpose so that each row is data from one chirp
% 
% % -------------------------------------------------------------------------------------------------------
% 
% % 计算切割条件
% cutInterval = 3 * 400; % 每隔400*3个chirp进行切割
% framesToCut = 24;   % 每次切掉8*3个chirp
% 
% % 对数据进行切割
% % 计算切割的位置：每隔400帧，切掉8帧
% totalFrames = size(LVDS, 1);  % Total number of chirps
% framesPerBlock = cutInterval + framesToCut;  % Total chirps per block (400 + 8)
% numBlocks = floor(totalFrames / framesPerBlock);  % Total blocks that can be processed
% 
% % 创建一个新的矩阵用于存储切割后的数据
% cutLVDS = [];
% 
% for blockIdx = 1:numBlocks
%     startFrame = (blockIdx - 1) * framesPerBlock + 1;
%     endFrame = startFrame + cutInterval - 1;
% 
%     % 获取每个块的前400帧数据
%     cutLVDS = [cutLVDS; LVDS(startFrame:endFrame, :)];
% end
% 
% [numChirps,~] = size(cutLVDS);
% % 将数据按RX重新组织
% retVal = zeros(numRX, numChirps * numADCSamples);
% for r_idx = 1:numRX
%     for c_idx = 1:numChirps
%         retVal(r_idx, (c_idx-1) * numADCSamples + 1:c_idx * numADCSamples) = ...
%             cutLVDS(c_idx, (r_idx-1) * numADCSamples + 1:r_idx * numADCSamples);
%     end
% end
% 
% end
