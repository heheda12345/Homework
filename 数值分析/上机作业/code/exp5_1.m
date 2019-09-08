solve([5, -4, 1;
       -4, 6, -4;
       1, -4, 7])
solve([25, -41, 10, -6;
       -41, 68, -17, 10;
       10, -17, 5, -3;
       -6, 10, -3, 2])

function [] = solve(A)
    n = length(A);
    x = ones(n, 1);
    l_old = NaN;
    k = 0;
    while (1)
        x = A*x;
        k = k+1;
        l_new = max(abs(x));
        x = x/l_new;
        if (abs(l_old - l_new) < 1e-5), break; end
        l_old = l_new;
    end
    fprintf("eigenvalue:        %f\n", l_old);
    fprintf("eigenvector:       %f %f %f %f", x); fprintf("\n");
    fprintf("Value of A*x:      %f %f %f %f", (A*x)'); fprintf("\n");
    fprintf("Value of lambda*x: %f %f %f %f", (l_old*x)); fprintf("\n\n");
end