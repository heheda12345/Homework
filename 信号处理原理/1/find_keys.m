function keys = find_keys(input_path, impl, exp_name)
    [data,fs]=audioread(input_path);
    tic
    frag_len = 2000;
    last_symbol = 'x'; % x is an invalid symbol
    keys = [];
    pos = [];
    for i = 1: frag_len: length(data)-frag_len
        [symbol, valid] = impl(data(i: i + frag_len), fs);
        if (valid == 0)
            last_symbol = 'x';
            continue;
        end
        if (last_symbol == symbol) % remove duplicate
            continue;
        end
        keys = [keys, symbol]; % save the key
        pos = [pos, i + frag_len/2];
        last_symbol = symbol;
    end
    toc
    f = figure();
    plot(1:length(data), data, pos, zeros(length(pos)), 'go');
    saveas(f, "figure/" + exp_name, "png");
end

