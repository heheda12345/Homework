calc = @damping_newton_method;
fprintf("Solving x^3-x-1=0\n");
solve(@(x) x^3-x-1, @(x) 3*x^2 - 1, 0.6);
fprintf("---------------------------------------------\n")
fprintf("Solving -x^3+5*x=0\n");
solve(@(x) -x^3+5*x, @(x) -3*x^2 + 5, 1.35);

function ret = solve(f, f_, x0)
    fprintf("not use damping\n");
    x = damping_newton_method(f, f_, x0, false);
    fprintf("root using ordinary newton method %.6f\n", x);
    fprintf("use damping\n");
    x = damping_newton_method(f, f_, x0, true);
    fprintf("root using damping newton method %.6f\n", x)
    fprintf("Answer using fzero: %f\n", fzero(f, x0))
    ret = x;
end

function ret = damping_newton_method(f, f_, x, use_damping)
    % f-the function; f_-derivative of f; x0-start point 
    e1 = 1e-8; % hyper-parameter
    e2 = 1e-8; % hyper-parameter
    lambda_initial = 0.9; % hyper-parameter
    k = 0;
    pre = x+1;
    while (abs(f(x))>e1 || abs(x-pre)>e2)
        pre = x;
        s = f(x)/f_(x);
        x = pre - s;
        i = 0;
        lambda = lambda_initial;
        if (use_damping)
            fprintf("  --iter 0 : lambda = %f x = %.10f f(x) = %.10f\n", lambda, x, f(x))
            while (abs(f(x) >= abs(f(pre))))
                x = pre - lambda * s;
                i = i + 1;
                lambda = lambda * 0.5;
                fprintf("  --iter %d : lambda = %f x = %.10f f(x) = %.10f\n", i, lambda, x, f(x))
            end
        end
        k = k+1;
        fprintf("  Step %d : x = %.10f f(x) = %.10f\n", k, x, f(x))
    end
    ret = x;
end

