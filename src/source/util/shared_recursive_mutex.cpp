#include <util\shared_recursive_mutex.h>

#include <mutex>

namespace citrus {

	// ------------------------------------------------------------------------
	// class: shared_mutex_base
	// ------------------------------------------------------------------------
	namespace detail {

		void shared_mutex_base::do_exclusive_lock(unique_lock &lk) {
			while(someone_has_exclusive_lock()) {
				m_exclusive_release.wait(lk);
			}

			take_exclusive_lock(); // We hold the mutex, there is no race here.

			while(number_of_readers() > 0) {
				m_shared_release.wait(lk);
			}
		}

		bool shared_mutex_base::do_exclusive_trylock(unique_lock &lk) {
			if(lk.owns_lock() && no_one_has_any_lock()) {
				take_exclusive_lock();
				return true;
			}
			return false;
		}

		void shared_mutex_base::do_lock_shared(unique_lock& lk) {
			while(someone_has_exclusive_lock() || maximal_number_of_readers_reached()) {
				m_exclusive_release.wait(lk);
			}
			increment_readers();
		}

		bool shared_mutex_base::do_try_lock_shared(unique_lock& lk) {
			if(lk.owns_lock() && !someone_has_exclusive_lock() &&
				!maximal_number_of_readers_reached()) {
				increment_readers();
				return true;
			}
			return false;
		}

		void shared_mutex_base::do_unlock_shared(scoped_lock& lk) {
			decrement_readers();

			if(someone_has_exclusive_lock()) { // Some one is waiting for us to unlock...
				if(number_of_readers() == 0) {
					// We were the last one they were waiting for, release one thread waiting
					// for
					// all shared locks to clear.
					m_shared_release.notify_one();
				}
			} else {
				// Nobody is waiting for shared locks to clear, if we were at the max
				// capacity,
				// release one thread waiting to obtain a shared lock in lock_shared().
				if(number_of_readers() == m_num_readers - 1)
					m_exclusive_release.notify_one();
			}
		}

		void shared_mutex_base::take_exclusive_lock() { m_state |= m_write_entered; }

		bool shared_mutex_base::someone_has_exclusive_lock() const {
			return (m_state & m_write_entered) != 0;
		}

		bool shared_mutex_base::no_one_has_any_lock() const { return m_state != 0; }

		unsigned shared_mutex_base::number_of_readers() const {
			return m_state & m_num_readers;
		}

		bool shared_mutex_base::maximal_number_of_readers_reached() const {
			return number_of_readers() == m_num_readers;
		}

		void shared_mutex_base::clear_lock_status() { m_state = 0; }

		void shared_mutex_base::increment_readers() {
			unsigned num_readers = number_of_readers() + 1;
			m_state &= ~m_num_readers;
			m_state |= num_readers;
		}

		void shared_mutex_base::decrement_readers() {
			unsigned num_readers = number_of_readers() - 1;
			m_state &= ~m_num_readers;
			m_state |= num_readers;
		}
	}

	// ------------------------------------------------------------------------
	// class: shared_mutex
	// ------------------------------------------------------------------------
	static_assert(std::is_standard_layout<shared_mutex>::value,
		"Shared mutex must be standard layout");

	void shared_mutex::lock() {
		std::unique_lock<std::recursive_mutex> lk(m_mutex);
		do_exclusive_lock(lk);
	}

	bool shared_mutex::try_lock() {
		std::unique_lock<std::recursive_mutex> lk(m_mutex, std::try_to_lock);
		return do_exclusive_trylock(lk);
	}

	void shared_mutex::unlock() {
		{
			std::lock_guard<std::recursive_mutex> lg(m_mutex);
			// We released an exclusive lock, no one else has a lock.
			clear_lock_status();
		}
		m_exclusive_release.notify_all();
	}

	void shared_mutex::lock_shared() {
		std::unique_lock<std::recursive_mutex> lk(m_mutex);
		do_lock_shared(lk);
	}

	bool shared_mutex::try_lock_shared() {
		std::unique_lock<std::recursive_mutex> lk(m_mutex, std::try_to_lock);
		return do_try_lock_shared(lk);
	}

	void shared_mutex::unlock_shared() {
		std::lock_guard<std::recursive_mutex> _(m_mutex);
		do_unlock_shared(_);
	}

	// ------------------------------------------------------------------------
	// class: shared_recursive_mutex
	// ------------------------------------------------------------------------
	void shared_recursive_mutex::lock() {
		std::unique_lock<std::recursive_mutex> lk(m_mutex);
		if(m_write_recurses == 0) {
			do_exclusive_lock(lk);
		} else {
			if(m_write_thread == std::this_thread::get_id()) {
				if(m_write_recurses ==
					std::numeric_limits<decltype(m_write_recurses)>::max()) {
					throw std::system_error(
						EOVERFLOW, std::system_category(),
						"Too many recursions in shared_recursive_mutex!");
				}
			} else {
				// Different thread trying to get a lock.
				do_exclusive_lock(lk);
			}
		}
		m_write_recurses++;
		m_write_thread = std::this_thread::get_id();
	}

	bool shared_recursive_mutex::try_lock() {
		std::unique_lock<std::recursive_mutex> lk(m_mutex, std::try_to_lock);
		if((lk.owns_lock() && m_write_recurses > 0 && m_write_thread == std::this_thread::get_id()) ||
			do_exclusive_trylock(lk)) {
			m_write_recurses++;
			m_write_thread = std::this_thread::get_id();
			return true;
		}
		return false;
	}

	void shared_recursive_mutex::unlock() {
		bool notify_them = false;
		{
			std::lock_guard<std::recursive_mutex> lg(m_mutex);
			if(m_write_recurses == 0) {
				throw std::system_error(ENOLCK, std::system_category(),
					"Unlocking a unlocked mutex!");
			}
			m_write_recurses--;
			if(m_write_recurses == 0) {
				// We released an exclusive lock, no one else has a lock.
				clear_lock_status();
				notify_them = true;
			}
		}
		if(notify_them) {
			m_exclusive_release.notify_all();
		}
	}

	void shared_recursive_mutex::lock_shared() {
		std::unique_lock<std::recursive_mutex> lk(m_mutex);
		do_lock_shared(lk);
	}

	bool shared_recursive_mutex::try_lock_shared() {
		std::unique_lock<std::recursive_mutex> lk(m_mutex, std::try_to_lock);
		return do_try_lock_shared(lk);
	}

	void shared_recursive_mutex::unlock_shared() {
		std::lock_guard<std::recursive_mutex> _(m_mutex);
		return do_unlock_shared(_);
	}

	int shared_recursive_mutex::num_write_locks() {
		std::lock_guard<std::recursive_mutex> _(m_mutex);
		return m_write_recurses;
	}

	bool shared_recursive_mutex::is_locked_by_me() {
		std::lock_guard<std::recursive_mutex> _(m_mutex);
		return m_write_recurses > 0 && m_write_thread == std::this_thread::get_id();
	}
}