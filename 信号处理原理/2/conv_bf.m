function conv = conv_bf(a, b)
    conv = zeros(1, length(a) + length(b) - 1);
    for i = 1: length(a)
        conv(i: i + length(b) - 1) = conv(i: i + length(b) - 1) + a(i) * b;
    end
end

