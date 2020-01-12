function conv = conv_overlap_save(a, b)
    if (length(a) < length(b))
        swap(a, b);
    end
    la = length(a);
    lb = length(b);
    overlap = lb - 1;
    L = min(10 * length(b), length(a) + length(b) - 1); % 取这样的L比较快
    m = 1;
    while (m < L)
        m = m * 2;
    end % 对齐到2^x
    step_size = m - overlap;
    dft_b = fft(b, m);
    a = [zeros(1, overlap), a];
    conv = zeros(1, length(a) + length(b) - 1 + m); 
    for i = 1: step_size: length(a) % 执行过程
        dft_a = fft(a(i : min(i + m - 1, length(a))), m);
        dft_ans = dft_a .* dft_b;
        ans_part = ifft(dft_ans);
        conv(i: i + step_size - 1) = ans_part(lb: m); 
    end
    conv = conv(1: la + lb - 1);
end

