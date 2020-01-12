function decode()
    [x, fs] = audioread("data/merge.m4a"); % 读入
    plot(x);
    f = fft(x); % 傅里叶变换
    fig = figure('Visible', 'off');
    plot(1:length(f), abs(f), "r");
    saveas(fig, "figure/recover-freq-merge.png");

    get_from(f, fs, 0, "Record1");
    get_from(f, fs, 1, "Record2");
    get_from(f, fs, 2, "Record3");
end

function get_from(f, fs, bias, name) % 提取出第bias段音频
    L = length(f);
    frag = 20000;
    padding = zeros(L-2*frag, 2);
    ff = [f(bias * frag + 1: (bias + 1) * frag, :); padding; f(L - (bias + 1) * frag + 1: L - bias * frag, :)]; % 恢复频域信号
    fig = figure('Visible', 'off');
    plot(1:length(ff), abs(ff), "r");
    saveas(fig, "figure/recover-freq-" + name + ".png");
    x = real(ifft(ff)); % 恢复时域信号
    plot(x, "b");
    saveas(fig, "figure/recover-time-" + name + ".png");
    audiowrite("data/recover-" + name + ".m4a", x, fs); % 保存
end