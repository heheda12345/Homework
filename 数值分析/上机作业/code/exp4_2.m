solve(1, 0.5, 100, 1.95)
solve(0.1, 0.5, 100, 1.9)
solve(0.01, 0.5, 100, 1.5)
solve(0.0001, 0.5, 100, 1.01)

function [] = solve(epsilon, a, n, w)
    fprintf("Now solving %f, choose w = %f\n", a, w);
    [A,b] = getMatrix(epsilon, a, n);
    y = precise(epsilon, a, n);
    fprintf("%f %f %f %f %f %f\n", y');
    fprintf("\n");
    y1 = jacobi(A, b);
    y2 = G_S(A, b);
    y3 = SOR(A, b, w);
    fprintf("1-norm: %f %f %f\n\n", max(abs(y-y1)), max(abs(y-y2)), max(abs(y-y3)))
end

function [A, b] = getMatrix(epsilon, a, n)
    A = diag(repmat(epsilon, 1, n-2), -1) + diag(repmat(-(2*epsilon+1/n), 1, n-1), 0) ...
        + diag(repmat(epsilon+1/n, 1, n-2), 1);
    b = repmat(a/n/n, n-1, 1);
    b(n-1) = b(n-1)-epsilon-1/n;
end

function y = precise(epsilon, a, n)
    x = (1:n-1)/n;
    y = (1-a)/(1-exp(-1/epsilon))*(1-exp(-x/epsilon))+a*x;
    y = y';
end

function x = jacobi(A, b)
    D = diag(A);
    a1 = [0; diag(A, -1)];
    a2 = [diag(A, 1); 0];
    n= length(b);
    x = zeros(n, 1);
    r = 0;
    while (1)
        r = r+1;
        x_new = (b - a1 .* [0; x(1:n-1)] - a2 .* [x(2:n); 0]) ./ D;
        st1 = num2str(x_new, '%.4g');
        st2 = num2str(x, '%.4g');
        if (all(size(st1)==size(st2)) && all(all(st1 == st2))), break; end
        x = x_new;
    end
    fprintf("Jacobi: run %d iterations\n", r)
end

function x = G_S(A, b)
    n = length(b);
    x = zeros(n, 1);
    r = 0;
    while (1)
        x_pre = x;
        r = r+1;
        x(1) = (b(1) - A(1,2) * x(2)) / A(1,1);
        for i = 2:n-1
            x(i) = (b(i) - A(i,i-1)*x(i-1) - A(i,i+1)*x(i+1)) / A(i,i);
        end
        x(n) = (b(n) - A(n,n-1)*x(n-1)) / A(n,n);
        x
        st1 = num2str(x_pre, '%.4g');
        st2 = num2str(x, '%.4g');
        if (all(size(st1)==size(st2)) && all(all(st1 == st2))), break; end
    end
    fprintf("G_S: run %d iterations\n", r)
end

function x = SOR(A, b, w)
    n = length(b);
    x = zeros(n, 1);
    r = 0;
    while (1)
        x_pre = x;
        r = r+1;
        x(1) = (1-w)*x(1) + w * (b(1) - A(1,2) * x(2)) / A(1,1);
        for i = 2:n-1
            x(i) = (1-w)*x(i) + w * (b(i) - A(i,i-1)*x(i-1) - A(i,i+1)*x(i+1)) / A(i,i);
        end
        x(n) = (1-w)*x(n) + w * (b(n) - A(n,n-1)*x(n-1)) / A(n,n);
        st1 = num2str(x_pre, '%.4g');
        st2 = num2str(x, '%.4g');
        if (all(size(st1)==size(st2)) && all(all(st1 == st2))), break; end
    end
    fprintf("SOR: run %d iterations\n", r)
end

function [D, L, U] = split(A)
    D = diag(diag(A), 0);
    L = tril(A, -1);
    U = triu(A, 1);
end

function r = converge_round(A)
    rho = max(abs(eig(A)));
    r = ceil(4/-log10(rho));
end
    
    