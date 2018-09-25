#include <stdarg.h>
#include <ctype.h>
#include <string.h>
// Now supported:
// %d %x %X %u %o
// %ld %lx %lX %lu %lo
// %f %e %E
// %s %c
// %p
// %%
// + 0 " " -

static struct {
	int padding_width;
	int padding_width_double;
	char sign;
	char padding_char;
	int upper_case;
	int left_align;
} property;

static int setSign(char *buf) {
	int cnt = 0;
	if( property.sign != 0 ) {
		buf[cnt] = property.sign;
		cnt++;
	}
	return cnt;
}

static int setPadding(char *buf) {
	int cnt = 0;
	while( cnt < property.padding_width ) {
		buf[cnt] = property.padding_char;
		cnt++;
	}
	return cnt;
}

static void shiftToRight(char *buf, int ep1, int ep2) {
	for(int i=1; i<=ep1; i++){
		buf[ep2 - i] ^= buf[ep1 - i];
		buf[ep1 - i] ^= buf[ep2 - i];
		buf[ep2 - i] ^= buf[ep1 - i];
	}
}

static unsigned int log10abs(long x);
char *intToStr(char *buf, long num, int base) {
	int cnt = 0;
	do {
		buf[cnt] = num % base;
		num /= base;
		cnt++;
	} while( num != 0 );
	int ep1 = cnt;

	property.padding_width -= cnt;
	if( property.sign != 0 ) {
		property.padding_width--;
	}
	if( property.padding_char == '0' ) {
		cnt += setPadding(buf + cnt);
		cnt += setSign(buf + cnt);
	}else{
		cnt += setSign(buf + cnt);
		cnt += setPadding(buf + cnt);
		if( property.left_align ) {
			shiftToRight(buf, ep1, cnt);
		}
	}
	
	for(int i=0; i<cnt/2; i++) {
		buf[i] ^= buf[cnt - i - 1];
		buf[cnt - i - 1] ^= buf[i];
		buf[i] ^= buf[cnt - i - 1];
	}

	for(int i=0; i<cnt; i++) {
		if( buf[i] <= 9 ) {
			buf[i] += '0';
		}else{
			if( buf[i] != ' ' && buf[i] != '0' && buf[i] != property.sign ) {
				if( property.upper_case ) {
					buf[i] += 'A' - 10;
				}else{
					buf[i] += 'a' - 10;
				}
			}
		}
	}
	return buf + cnt;
}

static unsigned int log10abs(long x) {
	int ret = 0;
	if( x < 0 ) x = -x;
	while( (x /= 10) != 0 ) ret++;
	return ret;
}

static unsigned long pow_10(int n) {
	unsigned long ret = 1;
	for(int i=0; i<n; i++){
		ret *= 10;
	}
	return ret;
}

static void process_minus_int(int *num) {
	if( *num < 0 ) {
		property.sign = '-';
		*num = -*num;
	}
}
static void process_minus_long(long *num) {
	if( *num < 0 ) {
		property.sign = '-';
		*num = -*num;
	}
}
static void process_minus_double(double *num) {
	if( *num < 0 ) {
		property.sign = '-';
		*num = -*num;
	}
}

static char *assign_decimal(char *buf, va_list *arg) {
	int num = va_arg(*arg, int);
	process_minus_int(&num);
	return intToStr(buf, num, 10);
}
static char *assign_unsigned_decimal(char *buf, va_list *arg) {
	unsigned int num = va_arg(*arg, int);
	return intToStr(buf, num, 10);
}
static char *assign_unsigned_hex(char *buf, va_list *arg) {
	unsigned int num = va_arg(*arg, int);
	return intToStr(buf, num, 16);
}
static char *assign_unsigned_oct(char *buf, va_list *arg) {
	unsigned int num = va_arg(*arg, int);
	return intToStr(buf, num, 8);
}
static char *assign_pointer(char *buf, va_list *arg) {
	*buf++ = '0';
	*buf++ = 'x';
	unsigned int num = va_arg(*arg, int);
	return intToStr(buf, num, 16);
}
static char *assign_long_decimal(char *buf, va_list *arg) {
	long num = va_arg(*arg, long);
	process_minus_long(&num);
	return intToStr(buf, num, 10);
}
static char *assign_long_unsigned_decimal(char *buf, va_list *arg) {
	unsigned long num = va_arg(*arg, long);
	return intToStr(buf, num, 10);
}
static char *assign_long_unsigned_hex(char *buf, va_list *arg) {
	unsigned long num = va_arg(*arg, long);
	return intToStr(buf, num, 16);
}
static char *assign_long_unsigned_oct(char *buf, va_list *arg) {
	unsigned long num = va_arg(*arg, long);
	return intToStr(buf, num, 8);
}
static char *assign_char(char *buf, va_list *arg) {
	char c = va_arg(*arg, int);
	if( !property.left_align ) {
		for(int i=0; i<property.padding_width - 1; i++){
			*buf++ = ' ';
		}
	}
	*buf++ = c;
	if( property.left_align ) {
		for(int i=0; i<property.padding_width - 1; i++){
			*buf++ = ' ';
		}
	}
	return buf;
}
static char *assign_string(char *buf, va_list *arg) {
	char *str = va_arg(*arg, char *);
	int len = strlen(str);
	if( property.padding_width_double != -1 && len > property.padding_width_double ) {
		len = property.padding_width_double;
	}
	if( len < property.padding_width && !property.left_align ) {
		for(int i=0; i<property.padding_width - len; i++){
			*buf++ = ' ';
		}
	}
	strncpy(buf, str, len);
	buf += len;
	if( len < property.padding_width && property.left_align ) {
		for(int i=0; i<property.padding_width - len; i++){
			*buf++ = ' ';
		}
	}
	return buf;
}
static char *assign_double(char *buf, va_list *arg) {
	double flt = va_arg(*arg, double);
	process_minus_double(&flt);
	if( property.padding_width_double == -1 ) {
		property.padding_width_double = 6;
	}

	property.padding_width -= property.padding_width_double;
	if( property.padding_width_double != 0 ) {
		property.padding_width--;
	}
	int padding_width_org = property.padding_width;
	int left_align_org = property.left_align;
	if( property.left_align ) {
		property.padding_width = 0;
		property.left_align = 0;
	}
	char *newb = intToStr(buf, flt, 10);
	property.padding_width = padding_width_org + (newb - buf);
	property.left_align = left_align_org;
	buf = newb;

	if( property.padding_width_double != 0 ) {
		*buf++ = '.';
	}

	property.sign = 0;
	property.padding_char = ' ';
	if( !property.left_align ) {
		property.padding_width = 0;
	}
	flt = flt * pow_10(property.padding_width_double) - (int)flt * pow_10(property.padding_width_double);
	buf = intToStr(buf, flt, 10);
	return buf;
}
static char *assign_double_imm(char *buf, double flt) {
	process_minus_double(&flt);
	if( property.padding_width_double == -1 ) {
		property.padding_width_double = 6;
	}

	property.padding_width -= property.padding_width_double;
	if( property.padding_width_double != 0 ) {
		property.padding_width--;
	}
	int padding_width_org = property.padding_width;
	int left_align_org = property.left_align;
	if( property.left_align ) {
		property.padding_width = 0;
		property.left_align = 0;
	}
	char *newb = intToStr(buf, flt, 10);
	property.padding_width = padding_width_org + (newb - buf);
	property.left_align = left_align_org;
	buf = newb;

	if( property.padding_width_double != 0 ) {
		*buf++ = '.';
	}

	property.sign = 0;
	property.padding_char = ' ';
	if( !property.left_align ) {
		property.padding_width = 0;
	}
	flt = flt * pow_10(property.padding_width_double) - (int)flt * pow_10(property.padding_width_double);
	buf = intToStr(buf, flt, 10);
	return buf;
}
static char *assign_exponential(char *buf, va_list *arg) {
	double flt = va_arg(*arg, double);
	process_minus_double(&flt);
	double tmp = flt;

	int digit_exponent;
	int exponent_pls, exponent = 0;
	int digit_all;

	double adj;

	if( 0.0 <= tmp && tmp < 1.0 ) {
		adj = 10.0;
		exponent_pls = -1;
	}else{
		adj = 0.1;
		exponent_pls = 1;
	}
	while( !(1.0 <= tmp && tmp < 10.0) ) {
		tmp *= adj;
		exponent += exponent_pls;
	}
	digit_exponent = log10abs(exponent) + 1;
	if( digit_exponent == 1 ) digit_exponent = 2 + 1;
	property.padding_width -= 1 + digit_exponent;

	int padding_width_org = property.padding_width;
	int left_align_org = property.left_align;
	if( property.left_align ) {
		property.padding_width = 0;
	}
	property.left_align = 0;
	char *newb = assign_double_imm(buf, tmp);
	digit_all = newb - buf;
	buf = newb;
	if( property.upper_case ) {
		*buf++ = 'E';
	}else{
		*buf++ = 'e';
	}
	property.padding_width = padding_width_org;
	property.left_align = left_align_org;

	property.sign = '+';
	property.padding_char = '0';
	property.padding_width = digit_exponent;
	process_minus_int(&exponent);
	newb = intToStr(buf, exponent, 10);
	digit_all += newb - buf;
	buf = newb;
	if( property.left_align ) {
		for(int i=0; i<padding_width_org - digit_all + digit_exponent; i++){
			*buf++ = ' ';
		}
	}
	return buf;
}

static const char *get_numeric_arguments(const char *fmt) {
	// Flags
	if( *fmt == '+' ) {
		property.sign = '+';
		property.padding_char = ' ';
		fmt++;
	}else if( *fmt == '-' ) {
		property.left_align = 1;
		property.padding_char = ' ';
		fmt++;
	}else if( *fmt == ' ' ) {
		property.sign = ' ';
		property.padding_char = ' ';
		fmt++;
	}else if( *fmt == '0' ) {
		property.padding_char = '0';
		fmt++;
	}else{
		property.padding_char = ' ';
	}

	while( isdigit(*fmt) ) {
		property.padding_width *= 10;
		property.padding_width += *fmt - '0';
		fmt++;
	}

	if( *fmt == '.' ) {
		fmt++;
		property.padding_width_double = 0;
		while( isdigit(*fmt) ) {
			property.padding_width_double *= 10;
			property.padding_width_double += *fmt - '0';
			fmt++;
		}
	}
	return fmt;
}

static char *assign_format(char *buf, const char **fmt, va_list *arg) {
	property.padding_width = 0;
	property.padding_width_double = -1;
	property.sign = 0;
	property.padding_char = 0;
	property.upper_case = 0;
	property.left_align = 0;

	*fmt = get_numeric_arguments(*fmt);
	char conversion = **fmt;
	switch(conversion) {
		case 'd':
			return assign_decimal(buf, arg);
			break;
		case 'u':
			return assign_unsigned_decimal(buf, arg);
			break;
		case 'x':
			return assign_unsigned_hex(buf, arg);
			break;
		case 'X':
			property.upper_case = 1;
			return assign_unsigned_hex(buf, arg);
			break;
		case 'o':
			return assign_unsigned_oct(buf, arg);
			break;
		case 'p':
			return assign_pointer(buf, arg);
			break;
		case 'l':
			*fmt = *fmt + 1;
			if( **fmt == 'd' ) {
				return assign_long_decimal(buf, arg);
			}else if( **fmt == 'u' ) {
				return assign_long_unsigned_decimal(buf, arg);
			}else if( **fmt == 'x' ) {
				return assign_long_unsigned_hex(buf, arg);
			}else if( **fmt == 'X' ) {
				property.upper_case = 1;
				return assign_long_unsigned_hex(buf, arg);
			}else if( **fmt == 'o' ) {
				return assign_long_unsigned_oct(buf, arg);
			}
			break;
		case 'c':
			return assign_char(buf, arg);
			break;
		case 's':
			return assign_string(buf, arg);
			break;
		case 'f':
			return assign_double(buf, arg);
			break;
		case 'e':
			return assign_exponential(buf, arg);
			break;
		case 'E':
			property.upper_case = 1;
			return assign_exponential(buf, arg);
			break;
		case '%':
			*buf = '%';
			return buf + 1;
			break;
		default:
			return (void *)0;
			break;
	}
	return (void *)0;
}

__attribute__((format(printf, 2, 3)))
int em_sprintf(char *buf, const char *fmt, ...) {
	va_list arg;
	va_start(arg, fmt);
	char *buf_p = buf;
	const char *fmt_p = fmt;

	while( *fmt_p != '\0' ) {
		if( *fmt_p == '%' ) {
			fmt_p++;
			buf_p = assign_format(buf_p, &fmt_p, &arg);
		}else{
			*buf_p = *fmt_p;
			buf_p++;
		}
		fmt_p++;
	}
	*buf_p = '\0';

	va_end(arg);
	return buf_p - buf;
}

#ifdef _EM_SPRINTF_MAIN
#include <stdio.h>

// redefine `sprintf' as em_sprintf
#ifdef sprintf
#	undef sprintf
#endif
#define sprintf(...) em_sprintf(__VA_ARGS__)

int main(void){
	char buf[200];
	sprintf(buf, "%p [%+3d] [%+4d] [%-20.3E] [%-20f] [%-20lo] [%-20c] [%-20.2s]\n", buf, 0, 10, -1140000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000.514, 114.514, 11111111111, 'F', "hey");
	printf("%s", buf);
	return 0;
}
#endif
