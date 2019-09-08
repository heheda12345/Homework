h = logspace(-16, 0);
err = abs(cos(1) - (sin(1+h) - sin(1)) ./ h);
trunc = sin(1)*h/2;
round = 2 * 1e-16 ./ h;
loglog(h, err, h, trunc, ':', h, round, ':', h, trunc+round, '--'), xlabel('���� h'), ylabel('���')
xlim([1e-16, 1])
ylim([1e-17, 10])
legend('ʵ�������', '�ض����', '�������', '�������', 'Location','east')


