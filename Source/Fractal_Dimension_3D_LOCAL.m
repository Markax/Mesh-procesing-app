% log-log plot of the box-counting
n = [0.096593 0.110952 0.102548 0.100624 0.104658 0.105673 0.110766 0.119729 0.122741 0.138679 0.155067 0.160009 0.175161 0.173712 0.179591 0.187113 0.195919 0.203796 0.216895 0.227390 0.231673 0.236557 0.240494 0.243389 0.245827 0.250310 0.255133 0.260036 0.262843 0.266324 0.270710 0.273035 0.273974 0.276957 0.280406 0.283960 0.286744 0.291785 0.294298 0.296123 0.299394 0.301453 0.304474 0.306305 0.308668 0.310322 0.311554 0.313884 0.316318 0.317648 0.319276 0.320332 0.322380 0.323294 0.324709 0.325900 0.327397 0.328772 0.330166 0.331255];
r = [1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17	18	19	20	21	22	23	24	25	26	27	28	29	30	31	32	33	34	35	36	37	38	39	40	41	42	43	44	45	46	47	48	49	50	51	52	53	54	55	56	57	58	59	60];

% NORMALIZANDO 
n = n / n(max(r)); % normaliza en base al área asociada al vértice para la l máxima

ini = 21; % initial box size
fin = 40; % final box size

N = log(n)';
R = log(r)';

figure;
scatter(R, N);
hold on;
 
% linear regression computation
Rr = R(ini : fin); % R limited to the selected range of boxes
Nr = N(ini : fin); % N limited to the selected range of boxes
 
x = [ones(length(Rr), 1) Rr]; % adds a column of ones to Rr
b = x \ Nr; % b(1) is the y-intercept and b(2) is the slope of the line 
y = x * b; % linear regression
corr = 1 - sum((Nr - y).^2) / sum((Nr - mean(Nr)).^2); % correlation
 
% plots the regression line and the fractal dimension results
plot(Rr, y, 'r', 'LineWidth', 3);
xlabel('log(l)');
ylabel('log(normalized average area)');


