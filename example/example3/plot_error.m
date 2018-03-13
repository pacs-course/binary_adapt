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

%% teoric error profile for piecewise analytic function
hold on
%b = 2.7; % x^(2/3)+y^(2/3)
b = 1.5; % abs(sqrt(x^2+y^2)-1/3)^(2/3)
y = exp(-b*(A.data(:,1).^(1/3)));
plot(A.data(:,1), y, '--r', 'linewidth', 6)
legend({'$\mathcal{E}\left(T_N,\mathcal{P}_N\right)$','$e^{-b{N}^{\frac{1}{3}}}$'},'Interpreter','latex','Fontsize',40)
