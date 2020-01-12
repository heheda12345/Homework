function [symbol, valid] = impl_fft(x, fs)
    dtmt_row = [679, 770, 852, 941];
    dtmt_col = [1209, 1336, 1477, 1633];
    dtmt_symbol = ['1', '2', '3', 'A';
                   '4', '5', '6', 'B';
                   '7', '8', '9', 'C';
                   '*', '0', '#', 'D'];
    y = abs(fft(x, fs)); % fft
    [row_val, row_pos] = max(y(601:1000)); % which row?
    [col_val, col_pos] = max(y(1151:1700)); % which column?
    if (row_val < 0.8 || col_val < 0.8) % ignore noise
        valid = 0;
        symbol = 'x';
        return;
    end
    row_pos = row_pos + 600;
    [~, row_id] = min(abs(dtmt_row - row_pos)); % find the nearst
    col_pos = col_pos + 1150;
    [~, col_id] = min(abs(dtmt_col - col_pos)); % find the nearsh
    valid = 1;
    symbol = dtmt_symbol(row_id, col_id);
end

