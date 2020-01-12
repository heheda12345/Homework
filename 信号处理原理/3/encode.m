function [x, fs] = encode()
    % 获取频域信号
    [f1, fs] = get_freq("Recording1");
    [f2, ~] = get_freq("Recording2");
    [f3, ~] = get_freq("Recording3");
    L = length(f1);
    frag = 20000;
    f = [f1(1:frag, :); f2(1:frag, :); f3(1:frag, :); zeros(L-frag*6, 2); f3(L-frag+1: L, :); f2(L-frag+1: L, :); f1(L-frag+1: L, :)]; % 组合起来
    fig = figure('Visible', 'off');
    plot(1: length(f), abs(f), 'r');
    saveas(fig, "figure/merge-freq.png");
    x = ifft(f); % 获取时域信号
    audiowrite("data/merge.m4a", real(x), fs); % 保存
end

function [freq, fs] = get_freq(name)
    [x, fs] = audioread("data/" + name + ".m4a"); % 读入
    x = x(1:130047, :); % align
    fig = figure('Visible', 'off');
    plot(x, 'b');
    saveas(fig, "figure/input-time-"+name+".png"); 
    
    freq = fft(x); % 获取频域信号
    plot(1:length(freq), abs(freq), 'r');
    saveas(fig, "figure/input-freq-"+name+".png");
end
