function decode()
    [x, fs] = audioread("data/merge.m4a"); % ����
    plot(x);
    f = fft(x); % ����Ҷ�任
    fig = figure('Visible', 'off');
    plot(1:length(f), abs(f), "r");
    saveas(fig, "figure/recover-freq-merge.png");

    get_from(f, fs, 0, "Record1");
    get_from(f, fs, 1, "Record2");
    get_from(f, fs, 2, "Record3");
end

function get_from(f, fs, bias, name) % ��ȡ����bias����Ƶ
    L = length(f);
    frag = 20000;
    padding = zeros(L-2*frag, 2);
    ff = [f(bias * frag + 1: (bias + 1) * frag, :); padding; f(L - (bias + 1) * frag + 1: L - bias * frag, :)]; % �ָ�Ƶ���ź�
    fig = figure('Visible', 'off');
    plot(1:length(ff), abs(ff), "r");
    saveas(fig, "figure/recover-freq-" + name + ".png");
    x = real(ifft(ff)); % �ָ�ʱ���ź�
    plot(x, "b");
    saveas(fig, "figure/recover-time-" + name + ".png");
    audiowrite("data/recover-" + name + ".m4a", x, fs); % ����
end