#ifndef FETCH_HPP
#define FETCH_HPP

#include <libc/stdlib.hpp>

extern "C" void fetch(class Fetch *self, const char *path);
extern "C" void fetch_callback(class Fetch *self, char *file, unsigned int length);

/**
 * Represents a fetch request.
 * 
 */
struct Fetch {
protected:
    const char *url_;
public:
    /**
     * Constructs a fetch request with the given URL.
     * This constructor calls the javascript `fetch` function with the given
     * URL. When the fetch function returns, the `callback` method will be
     * called.
     */
    Fetch(const char *url): url_{url} {
        fetch(this, url);
    }

    /**
     * Callback for handling result of fetch request.
     * \param file: The file data
     * \param length: The length of the file
     */
    virtual void callback(char *file, unsigned int length) {};

    /**
     * This class is meant to be inherited from, so we will mark the
     * destructor as virtual.
     */
    virtual ~Fetch() = default;
};

#define isdigit(c) (c >= '0' && c <= '9')
#define isdigit(c) (c >= '0' && c <= '9')


/**
 * 
 * 
 */
struct File: public Fetch {
protected:
    char *buffer_ = nullptr;
    unsigned long length_;
public:
    
    using Fetch::Fetch;

    float atof(const char *s, int &loc) {
        double a = 0.0;
        int e = 0;
        int c;
        int sign = 1;

        if (s[loc] == '+') {
            loc++;
        } else if (s[loc] == '-') {
            sign = -1;
            loc++;
        }
        
        while (isdigit(s[loc])) {
            a *= 10.0;
            a += (s[loc] - '0');
            loc++;
        }

        if (s[loc] == '.') {
            loc++;

            while (isdigit(s[loc])) {
                a *= 10.0;
                a += (c - '0');
                e -= 1;
                loc++;
            }
        }

        if (s[loc] == 'e' || s[loc] == 'E') {
            loc++;

            int sign = 1;
            int i = 0;

            if (s[loc] == '+') {
                loc++;
            } else if (s[loc] == '-') {
                sign = -1;
                loc++;
            }

            while (isdigit(s[loc])) {
                i *= 10.0;
                i += (s[loc] - '0');
                loc++;
            }

            e += i * sign;
        }

        while (e > 0) {
            a *= 10.0;
            e--;
        }
        while (e < 0) {
            a *= 0.1;
            e++;
        }

        return sign * a;
    }

    /**
     * Callback for handling result of fetch request.
     * \param file: The file data
     * \param length: The length of the file
     */
    void callback(char *file, unsigned int length) override {
        // Forward the call to the base class
        Fetch::callback(file, length);
        
        // Copy file into an owned buffer for later use.
        length_ = length;
        buffer_ = (char *) malloc(length);
        memcpy(buffer_, file, length);
        int loc = 0;
        while (loc < length) {
            if (buffer_[loc] == ' ') {
                loc++;
            } if (isdigit(buffer_[loc]) || buffer_[loc] == '-' || buffer_[loc] == '+') {
                float f = atof(buffer_, loc);
                print_float(f);
            } else {
                print_char(buffer_[loc]);
                loc++;
            }
        }
    }

    ~File() {
        free(buffer_);
    }
};

#endif /* FETCH_HPP */