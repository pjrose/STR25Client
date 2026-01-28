#pragma once

#include <Arduino.h>

struct PresenceDelta
{
    String arrived[4];
    size_t arrivedCount = 0;
    String left[4];
    size_t leftCount = 0;
    size_t count = 0;
    size_t max = 0;
};

class PresenceTracker
{
public:
    explicit PresenceTracker(size_t maxTags)
        : _maxTags(maxTags)
    {
        clear();
    }

    PresenceDelta update(const String* nowTags, size_t nowCount)
    {
        PresenceDelta delta;
        delta.max = _maxTags;

        bool nowPresent[4] = {false, false, false, false};
        String nowClipped[4];
        size_t clippedCount = min(nowCount, _maxTags);
        for (size_t i = 0; i < clippedCount; ++i)
        {
            nowClipped[i] = nowTags[i];
        }

        for (size_t i = 0; i < clippedCount; ++i)
        {
            bool found = false;
            for (size_t j = 0; j < _maxTags; ++j)
            {
                if (_present[j] && _uids[j] == nowClipped[i])
                {
                    nowPresent[j] = true;
                    found = true;
                    break;
                }
            }
            if (!found)
            {
                delta.arrived[delta.arrivedCount++] = nowClipped[i];
            }
        }

        for (size_t j = 0; j < _maxTags; ++j)
        {
            if (_present[j] && !nowPresent[j])
            {
                delta.left[delta.leftCount++] = _uids[j];
            }
        }

        clear();
        for (size_t i = 0; i < clippedCount; ++i)
        {
            _uids[i] = nowClipped[i];
            _present[i] = true;
        }

        delta.count = clippedCount;
        return delta;
    }

private:
    void clear()
    {
        for (size_t i = 0; i < _maxTags; ++i)
        {
            _uids[i] = "";
            _present[i] = false;
        }
    }

    size_t _maxTags;
    String _uids[4];
    bool _present[4];
};
