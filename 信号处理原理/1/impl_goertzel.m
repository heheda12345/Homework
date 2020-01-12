function [symbol, valid] = impl_goertzel(x, fs)
    dtmt_freq = [679, 770, 852, 941, 1209, 1336, 1477, 1633];
    dtmt_symbol = ['1', '2', '3', 'A';
                   '4', '5', '6', 'B';
                   '7', '8', '9', 'C';
                   '*', '0', '#', 'D'];
               
    v_prepre = zeros(1, length(dtmt_freq)); % solve the 8 frequence simultaneously
    v_pre = zeros(1, length(dtmt_freq));
    w_2 = 2 * cos(2 * pi * dtmt_freq / fs);
    for i = 1: length(x)
        cur = x(i) + w_2 .* v_pre - v_prepre; % goertezl
        v_prepre = v_pre;
        v_pre = cur;
    end
    cur = v_pre .* v_pre + v_prepre .* v_prepre - w_2 .* v_pre .* v_prepre; % find the meaning in report
    [row_val, row_id] = max(cur(1:4));
    [col_val, col_id] = max(cur(5:8));
    if (row_val < 0.8 || col_val < 0.8) % ignore noise
        valid = 0;
        symbol = 'x';
        return;
    end
    valid = 1;
    symbol = dtmt_symbol(row_id, col_id);
end