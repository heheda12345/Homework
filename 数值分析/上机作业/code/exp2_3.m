x = [1:0.1:40];
ab = [[2 : 3 : 31]; [5 : 3 : 34]]';
f = @(x) besselj(0, x);
rt = zeros(1, 10);
for i = 1:10
    rt(i) = fzerotx(f, ab(i, :)); 
end
plot(x, f(x), rt, f(rt), 'o')
grid on
fprintf("%.6f %.6f % .6f %.6f %.6f\n", rt)

function b = fzerotx(F, ab)
    eps = 1e-8;
    a = ab(1);
    b = ab(2);
    fa = F(a);
    fb = F(b);
    if sign(fa) == sign(fb)
        error('Function must change sign on the interval');
    end
    c = a;
    fc = fa;
    d = b-c;
    e = d;
    
    while fb ~= 0
        if sign(fa) == sign(fb)
            a = c; fa = fc;
            d = b-c; e = d;
        end
        if abs(fa) < abs(fb)
            c = b; b = a; a = c; % swap(a, b)
            fc = fb; fb = fa; fa = fc; % swap(fa, fb)
        end
        m = 0.5 * (a-b);
        tol = 2.0 * eps * max(abs(b), 1.0);
        if (abs(m) <= tol) || (fb == 0.0)
            break
        end
        if (abs(e) < tol) || (abs(fc) <= abs(fb)) % binary search
            d = m; e = m;
        else
            s = fb/fc;
            if (a == c)
                p = 2.0 * m * s;
                q = 1.0 - s;
            else
                q = fc/fa;
                r = fb/fa;
                p = s * (2.0 * m * q * (q-r) - (b-c) * (r-1.0));
                q = (q-1.0) * (r-1.0) * (s-1.0);
            end
            if p > 0, q = -q; else p = -p; end
            if (2.0*p < 3.0*m*q-abs(tol*q)) && (p < abs(0.5*e*q))
                e = d; d = p/q;
            else
                d = m; e = m;
            end
        end
        c = b;
        fc = fb;
        if abs(d) > tol
            b = b+d;
        else
            b = b-sign(b-a) * tol;
        end
        fb = F(b);
    end
end