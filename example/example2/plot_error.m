clear all
close all
clc
%put here the error_data file
filename = 'results/RENAME_ME_parsed_functor/error_data'
delimiterIn = '\t';
headerlinesIn = 1;
A = importdata(filename,delimiterIn,headerlinesIn);
figure;
errors = A.data(:,2);
semilogy(A.data(:,1), errors, 'linewidth', 3);
%plot(A.data(:,1), log(errors), 'linewidth', 3);
xlabel ('# iterations', 'Fontsize', 22);
ylabel ('L^{2} error', 'Fontsize', 22);
title ('Error vs Complexity', 'Fontsize', 28);
ylim([1E-16 1])
set(gca, 'Fontsize', 18)
%% teoric error profile for sqrt function
%hold on
%b = 1.7;
%y = exp(-b*sqrt(A.data(:,1)));
%plot(A.data(:,1), y, '-r')
%%
[garbage, expr] = strtok(A.textdata, ' ');
temp = strcat('@(x)', expr);
temp_2 = cell2mat(strrep(temp, '^', '.^'));
temp_3 = strrep(temp_2, '*', '.*');
f = str2func(temp_3);

iter = 49;

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
plot(x, f(x), 'linewidth', 3);
xlabel ('x', 'Fontsize', 22);
ylabel ('f(x)', 'Fontsize', 22);
title ('Analitic function', 'Fontsize', 28);
set(gca, 'Fontsize', 18)

figure;
plot(x, f(x), 'linewidth', 2);
hold on;
plot(x, y, 'o' ,'linewidth', 2);
xlabel ('x', 'Fontsize', 22);
lgd=legend('f(x)', 'f_{p} (x)')
lgd.FontSize = 16
title(  {strcat('Complexity: ', int2str(iter)); strcat('Error = ', num2str(err))}, 'Fontsize', 28);
set(gca, 'Fontsize', 18)