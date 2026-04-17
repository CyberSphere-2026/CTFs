all: main

main:
	cargo build --release -q
	cp target/release/main .
	cargo clean -q
	rm -f Cargo.lock

clean:
	rm -f main
