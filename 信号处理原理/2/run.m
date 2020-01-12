testcases = [100, 1000, 10000];
mul = 100;
format long
time_bf = zeros(length(testcases), mul);
time_fft = zeros(length(testcases), mul);
time_add = zeros(length(testcases), mul);
time_save = zeros(length(testcases), mul);
for i = 1: length(testcases)
    for j = 1: mul
        fprintf("running %d %d\n", i, j);
        a = ones(1, testcases(i) * j);
        b = ones(1, testcases(i));
        if (length(a) <= 200000) % bruteforce is too slow. Test it on small cases only.
            time_bf(i, j) = test(a, b, @conv_bf);
        end
        time_fft(i, j) = test(a, b, @conv_fft);
        time_add(i, j) = test(a, b, @conv_overlap_add);
        time_save(i, j) = test(a, b, @conv_overlap_save);
    end
end
dlmwrite('time/bf.txt', time_bf, '\t');
dlmwrite('time/fft.txt', time_fft, '\t');
dlmwrite('time/add.txt', time_add, '\t');
dlmwrite('time/save.txt', time_save, '\t');
format short
function time = test(a, b, imp)
    start = tic;
    conv = imp(a, b);
    time = toc(start);
end