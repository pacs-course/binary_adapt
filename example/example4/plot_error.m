clear all
close all
clc
%put here the error_data file
filename = 'error_data'
delimiterIn = '\t';
headerlinesIn = 1;
A = importdata(filename,delimiterIn,headerlinesIn);

[garbage, expr] = strtok(A.textdata, ' ');
temp = strcat('@(x)', expr);
temp_2 = cell2mat(strrep(temp, '^', '.^'));
temp_3 = strrep(temp_2, '*', '.*');
f = str2func(temp_3);

algo_err = A.data(1,1);
manual_err = A.data(2,1);

name = 'manual';
%name = 'algo';

filename2 = strcat('projection_data_',name);

delimiterIn2 = '\t';
headerlinesIn2 = 1;
A2 = importdata(filename2,delimiterIn2,headerlinesIn2);

x = A2.data(:,1);
[x,I] = sort(x);
y = A2.data(:,2);
y = y(I);

figure;
plot(x, f(x), 'linewidth', 10);
hold on;
plot(x, y, '-r' ,'linewidth', 6);
xlabel ('x', 'Fontsize', 40);
lgd=legend('f(x)', 'f_{p} (x)')
lgd.FontSize = 40;
err = 0;
if (strcmp(name,'algo'))
    err = algo_err;
else
    err = manual_err;
end

iter=45;
    
exp = floor(log10(err));
coeff=err/10^exp;
title(  {strcat('$N = ', int2str(iter),'$, $\mathcal{E}_m = ', num2str(coeff), '\cdot 10^{',int2str(exp),'}$')},'Interpreter','latex', 'Fontsize', 50);
set(gca, 'Fontsize', 36)
