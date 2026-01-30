using System;
using System.Collections.Generic;
using System.Linq;

namespace St25R200.OpenSource;

public sealed class PresenceTracker
{
    private readonly int _maxTags;
    private HashSet<string> _present;

    public PresenceTracker(int maxTags)
    {
        if (maxTags <= 0) throw new ArgumentOutOfRangeException(nameof(maxTags));
        _maxTags = maxTags;
        _present = new HashSet<string>(StringComparer.OrdinalIgnoreCase);
    }

    public PresenceDelta Update(IEnumerable<string> nowPresent)
    {
        var set = new HashSet<string>(nowPresent, StringComparer.OrdinalIgnoreCase);
        int originalCount = set.Count;
        bool clipped = false;
        if (set.Count > _maxTags)
        {
            set = new HashSet<string>(set.Take(_maxTags), StringComparer.OrdinalIgnoreCase);
            clipped = true;
        }

        var arrived = set.Except(_present, StringComparer.OrdinalIgnoreCase).ToList();
        var left = _present.Except(set, StringComparer.OrdinalIgnoreCase).ToList();
        _present = set;

        return new PresenceDelta(arrived, left, set.Count, _maxTags, originalCount, clipped);
    }

    public sealed record PresenceDelta(IReadOnlyList<string> Arrived, IReadOnlyList<string> Left, int Count, int Max, int OriginalCount, bool Clipped);
}
