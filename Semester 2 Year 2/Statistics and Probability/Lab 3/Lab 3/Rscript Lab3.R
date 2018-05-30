norm_samp <- rnorm(1000, mean=0, sd=1)
hist(norm_samp, breaks = 30)
pnorm(1.2, mean = 0, sd = 1)
pnorm(0.6, mean = 0, sd = 1)
areaBetween0.6_2.1 <- (pnorm(1.2, mean = 0, sd = 1) - pnorm(0.6, mean = 0, sd = 1))

#elephants trunks
area1 <- pnorm(2.1, mean = 1.8, sd = 0.4)
area2 <- pnorm(1.6, mean = 1.8, sd = 0.4)
areaBetweenA1_A2 <- (pnorm(2.1, mean = 1.8, sd = 0.4) - pnorm(1.6, mean = 1.8, sd = 0.4))
AreaBetweenA1_A2 <- (area1-area2)


#FIND PERCENTILES
#FIND q VALUES WHEN GIVEN PROBABILITIES
A <- qnorm(p = 0.025, mean = 10 , sd = sqrt(3))
B <- qnorm(p = 0.975, mean = 10 , sd = sqrt(3))
B-A

#
variables <- read.table("variables.txt", header = TRUE)
variables <- data.frame(variables)
summary(variables$var1)
sd(variables$var1, na.rm = FALSE)
norm_samp_var1 <- rnorm(300, mean=20.29, sd=4.917031)
hist(variables, breaks = 100)
hist(variables$var1, breaks = 100)

summary(variables$var2)
sd(variables$var2, na.rm = FALSE)
norm_samp_var2 <- rnorm(300, mean=20.08, sd=19.91025)
hist(norm_samp_var2, breaks = 100)
hist(variables$var2, breaks = 100)

#ipad
samp_exp_ipad <- rexp(1000, rate=0.25)
hist(samp_exp_ipad, breaks = 100)
pexp(5, rate=0.25)
ipadLess_than_8 <- pexp(8, rate=0.2)
ipadLess_than_6 <- pexp(6, rate=0.2)
ipadBetween6_8 <- (ipadLess_than_8 - ipadLess_than_6)
ipadBetween6_8

rweibull(1000, shape=1.5, scale=2)
hist(rweibull(1000, shape=1.5, scale=2))
smartphone_prob_lessTHAN3 <- pweibull(3, shape=1.5, scale = 2, lower.tail = T, log.p = F)
1-smartphone_prob_lessTHAN3

xbar <- mean(variables[, 1])
s <- sd(variables[, 1])
z <- qnorm(0.90)
n <- length(variables[, 1])
lower <- xbar - z*(s/sqrt(n))
upper <- xbar + z*(s/sqrt(n))
lower
upper
xbar
