format long
n = [8, 10, 12];
disturb = [false, true];
for i = n
    for j = disturb
        calc(i,j)
    end
end
        
       
function [] = calc(n, disturb)
    fprintf('Experiment n=%d,', n);
    if (disturb)
        fprintf('do disturbing\n')
    else
        fprintf('no disturbing\n')
    end
    x = ones(n, 1);
    H = hilbert(n);
    fprintf("det %d : %f\n", n, det(H))
    b = H * x;
    if (disturb)
        b = b + 1e-7*ones(n, 1);
    end
    x_ = cholesky(H, b);
    r = b - H*x_;
    dx = x_-x;
    fprintf('root:\n');
    fprintf('%.15f %.15f %.15f\n', x_);
    if (mod(n,3)), fprintf('\n'); end
    fprintf('infinity norm of r: %.15f\n', infnorm(r));
    fprintf('infinity norm of dx: %.15f\n', infnorm(dx));
    fprintf('\n');
end
    
function H = hilbert(n)
    H = repmat(1:n, n, 1);
    H = 1 ./ (H + H' -1);
end

function x = cholesky(a, b)
    n = length(b);
    for j = 1:n
        a(j,j) = (a(j,j)-sumsqr(a(j, 1:j-1))).^0.5;
        for i = j+1:n
            a(i,j) = (a(i,j)-sum(a(i,1:j-1).*a(j,1:j-1)))/a(j,j);
        end
        a(1:j-1, j)=0;
    end
    y = front(a, b);
    x = back(a', y);
end

function b = front(a, b)
    n = length(b);
    for i = 1:n
        b(i) = b(i)/a(i,i);
        b(i+1:n) = b(i+1:n) - a(i+1:n,i)*b(i);
    end
end

function b = back(a, b)
    n = length(b);
    for i = n:-1:1
        b(i) = b(i)/a(i,i);
        b(1:i-1) = b(1:i-1) - a(1:i-1,i)*b(i);
    end
end

function ret = infnorm(x)
    ret = max(abs(x));
end
    