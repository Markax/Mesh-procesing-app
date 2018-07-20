% log-log plot of the box-counting
n = [22665.243205 23166.358735 25291.866288 26023.926689 27698.095954 29017.875820 30405.418628 32279.063440 33912.124651 35958.074228 38086.999948 39623.667812 41405.388462 43595.654621 45804.486592 47708.087064 49077.381759 51513.889826 53341.347412 55159.069634 56938.919835 58306.636297 59675.127142 60902.453939 61927.669000];
r = [1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17	18	19	20	21	22	23	24	25];

% NORMALIZANDO 
n = n / n(max(r)); % normaliza en base al área de la reconstrucción con l máxima

ini = 11; % initial box size
fin = 25; % final box size

N = log(n)';
R = log(r)';

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
ylabel('log(normalized area)');


