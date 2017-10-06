clear all
clc
filename = 'error_data'
delimiterIn = '\t';
headerlinesIn = 1;
A = importdata(filename,delimiterIn,headerlinesIn);
figure;
errors = A.data(:,2);
semilogy(A.data(:,1), errors, 'linewidth', 2);
xlabel ('# iterations');
ylabel ('L^{2} error');
title ('Error vs Complexity');
ylim([1E-16 1])

[garbage, expr] = strtok(A.textdata, ' ');
temp = strcat('@(x)', expr);
temp_2 = cell2mat(strrep(temp, '^', '.^'));
temp_3 = strrep(temp_2, '*', '.*');
f = str2func(temp_3);

iter = 10;

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
plot(x, f(x), 'linewidth', 2);
xlabel ('x');
ylabel ('f(x)');
title ('Analitic function');

figure;
plot(x, f(x), 'linewidth', 2);
hold on;
plot(x, y, 'linewidth', 2);
xlabel ('x');
legend('f(x)', 'f_{p} (x)')
title(  {strcat('Complexity: ', int2str(iter)); strcat('Error = ', num2str(err))});
