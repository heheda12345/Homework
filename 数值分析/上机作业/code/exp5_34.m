% A = [2.9766, 0.3945, 0.4198, 1.1159;
%      0.3945, 2.7328, -0.3097, 0.1129;
%      0.4198, -0.3097, 2.5675, 0.6079;
%      1.1159, 0.1129, 0.6079, 1.7231]; 

A = [0.5, 0.5, 0.5, 0.5;
     0.5, 0.5, -0.5, -0.5;
     0.5, -0.5, 0.5, -0.5;
     0.5, -0.5, -0.5, 0.5];

[a, b] = eig(A);
fprintf("standard\n");
disp(diag(b));
fprintf("=======basic QR========\n");
lambda = basicQR(A);
fprintf("=====QR with bias =====\n");
lambda = biasQR(A);
fprintf("lambda\n");
disp(lambda);


function s = mySign(x)
    s = sign(x);
    if s == 0
        s = 1;
    end
end

function lambda = biasQR(A)
    n = length(A);
    for i = 1:4
        s = A(n,n);
        [Q,R] = QR(A-s*eye(n));
        A = R*Q + s*eye(n);
        fprintf("Iter %d\n", i);
        disp(A);
    end
    lambda = diag(A);
end
        
    

function lambda = basicQR(A)
    for i = [1:1]
        [Q,R] = QR(A);
        A = R*Q;
        fprintf("Iter %d\n", i);
        Q
        R
        A
    end
    lambda = diag(A);
end

function [Q, R] = QR(A)
    n = length(A);
    R = A;
    Q = eye(n);
    for k = (1:n)
        sig = mySign(R(k, k)) * sqrt(sum(R(k:n, k).^2));
        v = R(k:n, k) + sig * [1; zeros(n-k, 1)];
        b = v' * v;
        w = [zeros(k-1, 1); v/sqrt(b)];
        Q = Q * (eye(n) - 2 * w * w'); 
        R(k:n, k:n) = R(k:n, k:n) - 2/b * v * (v' * R(k:n, k:n));
    end
end