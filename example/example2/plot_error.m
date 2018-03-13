clear all
close all
clc
%put here the error_data file
filename = 'error_data'
delimiterIn = '\t';
headerlinesIn = 1;
A = importdata(filename,delimiterIn,headerlinesIn);
figure;
errors = A.data(:,2);
semilogy(A.data(:,1), errors, 'linewidth', 10);
xlabel ('# iterations', 'Fontsize', 40);
ylabel ('L^{2} error', 'Fontsize', 40);
title ('Error vs Complexity', 'Fontsize', 50);
ylim([1E-16 1])
set(gca, 'Fontsize', 36)

%% teoric error profile for analytic function
hold on
r = 1.45;
r_vec = r*ones(length(A.data),1);
y = (r_vec.^(-2*A.data(:,1)))./A.data(:,1);
plot(A.data(:,1), y, '--r', 'linewidth', 6)
legend({'$\mathcal{E}\left(T_N,\mathcal{P}_N\right)$','$N^{-1} r^{-2N}$'},'Interpreter','latex','Fontsize',40)

%% teoric error profile for piecewise analytic function
hold on
b = 1.3;  %(sqrt(abs(x-1/3)))
%b = 1.7; %sqrt(x)
y = exp(-b*sqrt(A.data(:,1)));
plot(A.data(:,1), y, '--r', 'linewidth', 6)
legend({'$\mathcal{E}\left(T_N,\mathcal{P}_N\right)$','$e^{-b\sqrt{N}}$'},'Interpreter','latex','Fontsize',40)
%%
[garbage, expr] = strtok(A.textdata, ' ');
temp = strcat('@(x)', expr);
temp_2 = cell2mat(strrep(temp, '^', '.^'));
temp_3 = strrep(temp_2, '*', '.*');
f = str2func(temp_3);

iter = A.data(end,1);

s_iter = int2str(iter);
if iter<10
    s_iter = strcat(int2str(0), s_iter);
end
filename2 = strcat('projection_data_', s_iter)
delimiterIn2 = '\t';
headerlinesIn2 = 1;
A2 = importdata(filename2,delimiterIn2,headerlinesIn2);

x = A2.data(:,1);
[x,I] = sort(x);
y = A2.data(:,2);
y = y(I);
err = errors(iter + 1);

figure;
plot(x, f(x), 'linewidth', 8);
xlabel ('x', 'Fontsize', 40);
ylabel ('f(x)', 'Fontsize', 40);
title ('Analitic function', 'Fontsize', 50);
set(gca, 'Fontsize', 36)

figure;
plot(x, f(x), 'linewidth', 10);
hold on;
plot(x, y, '-r' ,'linewidth', 6);
xlabel ('x', 'Fontsize', 40);
lgd=legend('f(x)', 'f_{p} (x)')
lgd.FontSize = 40
exp = floor(log10(err));
coeff=err/10^exp;
title(  {strcat('$N = ', int2str(iter),'$, $\mathcal{E} = ', num2str(coeff), '\cdot 10^{',int2str(exp),'}$')},'Interpreter','latex', 'Fontsize', 50);
set(gca, 'Fontsize', 36)
