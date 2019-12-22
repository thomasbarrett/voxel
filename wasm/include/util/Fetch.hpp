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
    int loc_ = 0;
public:
    
    using Fetch::Fetch;

    bool hasNext() {
        return loc_ < length_;
    }

    char next() {
        return buffer_[loc_++];
    }

    char peek() {
        return buffer_[loc_];
    }

    bool next(const char *str) {
        int i = 0;
        while(str[i] != '\0' && loc_ + i < length_) {
            if (buffer_[loc_ + i] == str[i]) {
                i++;
            } else {
                return false;
            }
        }
        loc_ += i;
        return true;
    }

    

    int nextInt() {
        int num = 0;
        int sign = 1;

        while(buffer_[loc_] == ' ' || buffer_[loc_] == '\n') {
            loc_++;
        }

        if (buffer_[loc_]== '+') {
            loc_++;
        } else if (buffer_[loc_]== '-') {
            sign = -1;
            loc_++;
        }

        while (isdigit(buffer_[loc_])) {
            num *= 10;
            num += (buffer_[loc_] - '0');
            loc_++;
        }

        return num * sign;
    }

    float nextFloat() {
        double a = 0.0;
        int e = 0;
        int sign = 1;

        while(buffer_[loc_] == ' ' || buffer_[loc_] == '\n') {
            loc_++;
        }

        if (buffer_[loc_]== '+') {
            loc_++;
        } else if (buffer_[loc_]== '-') {
            sign = -1;
            loc_++;
        }
        
        while (isdigit(buffer_[loc_])) {
            a *= 10.0;
            a += (buffer_[loc_]- '0');
            loc_++;
        }

        if (buffer_[loc_]== '.') {
            loc_++;

            while (isdigit(buffer_[loc_])) {
                a *= 10.0;
                a += (buffer_[loc_] - '0');
                e -= 1;
                loc_++;
            }
        }

        if (buffer_[loc_]== 'e' || buffer_[loc_]== 'E') {
            loc_++;

            int sign = 1;
            int i = 0;

            if (buffer_[loc_]== '+') {
                loc_++;
            } else if (buffer_[loc_]== '-') {
                sign = -1;
                loc_++;
            }

            while (isdigit(buffer_[loc_])) {
                i *= 10.0;
                i += (buffer_[loc_]- '0');
                loc_++;
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
    virtual void callback(char *file, unsigned int length) override {
        // Forward the call to the base class
        Fetch::callback(file, length);
        
        // Copy file into an owned buffer for later use.
        length_ = length;
        buffer_ = (char *) malloc(length);
        memcpy(buffer_, file, length);
    }

    ~File() {
        free(buffer_);
    }
};

#endif /* FETCH_HPP */