fprintf("----------question 1----------\n");
sum = single(0);
n = single(0);
while (1)
    n = n + 1;
    cur = sum + single(1/n);
    if (cur == sum)
        break
    end
    sum = cur;
end
fprintf("end at %d : %f\n", n, sum)
fprintf("----------question 2----------\n");
sum_acc = 0;
n = double(n);
for i = 1:n
    sum_acc = sum_acc + 1/i;
end
fprintf("sum %f\nerror %f\nrelative error %f\n", sum_acc, sum-sum_acc, (sum-sum_acc)/sum_acc)
fprintf("----------question 3----------\n");
tic
n = 1e10;
sum_acc = 0;
for i = 1 : n
    sum_acc = sum_acc + 1/i;
end
t = toc;
fprintf('time used: %fs\n', t)


