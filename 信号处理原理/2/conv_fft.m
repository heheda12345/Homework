function conv = conv_fft(a, b)
    n = length(a) + length(b) - 1;
    m = 1;
    while (m < n)
        m = m + m;
    end % ¶ÔÆëµ½ 2^x
    dft_a = fft(a, m);
    dft_b = fft(b, m);
    dft_mul = dft_a .* dft_b;
    conv = ifft(dft_mul);
    conv = conv(1:n);
end

