function conv = conv_overlap_add(a,b)
    if (length(a) < length(b))
        swap(a, b);
    end
    L = min(4 * length(b), length(a) + length(b) - 1); % 取成这样比较快
    m = 1;
    while (m < L)
        m = m * 2;
    end
    L = m - length(b) + 1; % 对齐到2^x
    dft_b = fft(b, m);
    conv = zeros(1, length(a) + length(b) - 1 + m);
    for i = 1: L: length(a) % 执行过程
        a_part = a(i: min(i + L - 1, length(a)));
        dft_a = fft(a_part, m);
        dft_ans = dft_a .* dft_b;
        ans_part = ifft(dft_ans);
        conv(i: i + m - 1) = conv(i: i + m - 1) + ans_part;
    end
    conv = conv(1: length(a) + length(b) - 1);
end

